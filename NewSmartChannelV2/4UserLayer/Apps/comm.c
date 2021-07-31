/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : comm.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��6��18��
  ����޸�   :
  ��������   : ��������ָ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "comm.h"
#include "bsp_led.h"
#include "malloc.h"
#include "ini.h"
#include "bsp_uart_fifo.h"
#include "Devinfo.h"
#include "bsp_uart.h"
#include "pub_options.h"
#include "led_task.h"
#include "log.h"


/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/

MOTORCTRL_QUEUE_T gMotorCtrlQueue,gRecvMotorCtrlQueue;    //����һ���ṹ��������Ϣ���У�����ͬ��������Ӧ����
MOTORCTRL_QUEUE_T gSecMotorCtrlQueue;    //����һ���ṹ��������Ϣ���У�����ͬ��������Ӧ����
TOHOST_QUEUE_T     gToHostQueueBuf,gToHostQueueBuf_recv;



RECVHOST_T gRecvHost;
static uint16_t crc_value = 0;

//uint32_t gTime1 = 0;
//uint32_t gTime2 = 0;

static SYSERRORCODE_E parseJSON(uint8_t *text,CMD_RX_T *cmd_rx); //˽�к���
static uint16_t  packetJSON(CMD_TX_T *cmd_tx,uint8_t *command_data);
static uint16_t  packetDeviceInfo(uint8_t *command_data);
static void parseMotorParam(CMD_RX_T *cmd_rx);
static void SetLedShowInfo(CMD_RX_T *cmd_rx,char mode);
static void packetToMotorA(uint8_t cmd,uint8_t *buff,uint8_t len);


//static void displayTask(void);



/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

    
void init_serial_boot(void)
{
    crc_value = 0;    
    memset(&gRecvHost,0x00,sizeof(gRecvHost));
}

/*****************************************************************************
 �� �� ��  : deal_Serial_Parse
 ��������  :     ���յ������ݽ��������ݰ���
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ɺ���

*****************************************************************************/
// 7E 01 A1 00 AA 74 
// 7E 01 A1 AA 25 48 00 AA 74 7E 01 A1 00 AA 74


void deal_Serial_Parse(void)
{
    uint8_t ch = 0; 
    
    while(1)
    {  
//        if(bsp_Usart1_RecvOne(&ch) != 1)  //��ȡ��������
//        if(comGetChar(COM1, &ch) != 1)    //��ȡ��������

        if(BSP_UartRead(SCOM1,&ch,1)!=1)
//        if(bsp_Usart1_RecvOne(&ch) != 1)  //��ȡ��������
        {
            delay_ms(10);
            
            return;
        }

        
        switch (gRecvHost.RxdStatus)
        { /*��������״̬*/                
            case 0:
                gRecvHost.RxdStatus = SPACE;
                if(STX == ch) /*���հ�ͷ*/
                {
                    gRecvHost.RxdBuf[0] = ch;
                    gRecvHost.NowLen = 1;
                    gRecvHost.RxdStatus = 20;
                }
                break;              
            case 20:      /* �����������ݰ� */
                if (ch == ETX) 
                {
                    gRecvHost.RxdStatus = 21;
                }
                gRecvHost.RxdBuf[gRecvHost.NowLen++] = ch; //ETX

                break;
            case 21:
                crc_value = CRC16_Modbus((uint8_t *)gRecvHost.RxdBuf, gRecvHost.NowLen);                
                gRecvHost.RxCRCHi = crc_value>>8;
                gRecvHost.RxCRCLo = crc_value & 0xFF;                
                gRecvHost.RxdStatus = 22;
                gRecvHost.RxdBuf[gRecvHost.NowLen++] = ch; //CRCHI
                
                break;
           case 22:
                gRecvHost.RxdBuf[gRecvHost.NowLen++] = ch; //CRCLO
                
                if(gRecvHost.RxdBuf[gRecvHost.NowLen-2] == gRecvHost.RxCRCHi && \
                    gRecvHost.RxdBuf[gRecvHost.NowLen-1] == gRecvHost.RxCRCLo )
                {    
                    gRecvHost.RxdTotalLen = gRecvHost.NowLen;
                    gRecvHost.RxdFrameStatus = FINISH;                    
                    gRecvHost.RxdStatus = 0;
                   
                    break;
                }
                else
                {
                    gRecvHost.RxdFrameStatus = FINISH;
                    gRecvHost.RxdBuf[gRecvHost.NowLen++] = ch;
                    gRecvHost.RxdStatus = 20;
                    crc_value = 0;
                    DBG("CRC check error,The correct value should be:HI=%02x,LO=%02x\r\n",gRecvHost.RxCRCHi,gRecvHost.RxCRCLo);
                }

                break;
               
            default:                
                if (gRecvHost.RxdFrameStatus == SPACE) 
                {
                    gRecvHost.RxdFrameStatus = FINISH;
                    gRecvHost.RxdStatus = 0;
                }
                break;
         }
    }
}






/*****************************************************************************
 �� �� ��  : deal_rx_data
 ��������  : �����յ����������ݰ�������Ӧ���ݰ���
 �������  : void
 �������  : ��
 �� �� ֵ  : void
 ���ú���  : 
 ��������  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ɺ���

*****************************************************************************/
void deal_rx_data(void)
{
    uint8_t bccHi = 0;       
    uint8_t bccLo = 0; 


    uint8_t json_buf[JSON_PACK_MAX] = {0};
    CMD_RX_T cmd_rx;

    memset(&cmd_rx,0x00,sizeof(CMD_RX_T));  
    
    if (FINISH == gRecvHost.RxdFrameStatus)
    {        
        if(gRecvHost.RxdBuf[0] == STX && gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -3] == ETX)                                   //����02���ݰ�
        {    
//            dbh("recv deal_rx_data",(char *)gRecvHost.RxdBuf,gRecvHost.RxdTotalLen);
            
            //����У��ֵ
            bccHi =  crc_value >> 8;
            bccLo = crc_value & 0xff;
            
            if ((bccHi == gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -2]) && \
                (bccLo == gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -1]))
            {
				//ת��JSON���ݰ�
                memcpy(json_buf,(const char*)gRecvHost.RxdBuf+1,gRecvHost.RxdTotalLen-4);
//                DBG("recv json data = : %s\r\n",json_buf);

                //����JSON���ݰ�              
                if(parseJSON(json_buf,&cmd_rx) == NO_ERR)
                {
                    send_to_device(&cmd_rx);
                }
                else
                {
                    //ָ�����ʧ�ܣ�����λ�����ͽ���ʧ�ܵ�״̬��Ҫ���ط�
                     DBG("parseJSON error\r\n");
//                    SendAsciiCodeToHost(ERRORINFO,COMM_PARSE_ERR,"cmd parse error");
                }

                init_serial_boot();   


            }
            else
            {
                DBG("CRC ERROR\r\n");
//                dbh("CRC ERROR RxdBuf", (char *)gRecvHost.RxdBuf, gRecvHost.RxdTotalLen);
//                DBG("bccHi = %02x,bccLo = %02x",bccHi,bccLo);
//                SendAsciiCodeToHost(ERRORINFO,COMM_CRC_ERR,"deal rx data crc error");

                init_serial_boot();
              
            }
        }
        else
        {
//            DBG("-----------execute deal_rx_data-----------\r\n");
            init_serial_boot();
        }
    }

}


SYSERRORCODE_E send_to_host(uint8_t cmd,uint8_t *buf,uint8_t len)
{
    uint16_t i = 0;
    uint16_t json_len = 0;
    uint8_t TxdBuf[MAX_TXD_BUF_LEN]={0};
    uint8_t tmpBuf[MAX_TXD_BUF_LEN] = {0};
    //uint16_t iCRC = 0;
    CMD_TX_T cmd_tx;


    memset(tmpBuf,0x00,sizeof(tmpBuf));
    memset(TxdBuf,0x00,sizeof(TxdBuf));
    memset(&cmd_tx,0x00,sizeof(CMD_TX_T)); 
    
    i = 3;
    TxdBuf[0] = STX;
    cmd_tx.cmd = cmd;
    cmd_tx.code = 0;
    
    bcd2asc(cmd_tx.data, buf, len*2, 1); 
    json_len = packetJSON(&cmd_tx,tmpBuf);
    
    if(json_len == 0)
    {        
        return CJSON_PACKET_ERR;
    }
    else
    {
        i += json_len;
    }
    memcpy(TxdBuf+3,tmpBuf,i-3); 
    TxdBuf[i++] = ETX;   

    TxdBuf[1] = i>>8; //high
    TxdBuf[2] = i&0xFF; //low

//    //2020.11.09 �޸ģ���Ϊ��λ�������飬����У��ֵ�п��ܸ���ͷ��ͻ����������Ϊ�̶�ֵ
//    iCRC = CRC16_Modbus(TxdBuf, i);  
//    TxdBuf[i++] = iCRC >> 8;
//    TxdBuf[i++] = iCRC & 0xff;  

    TxdBuf[i++] = 0xA5;
    TxdBuf[i++] = 0xA5;  


//    DBG("send json data = %s\r\n",tmpBuf);
//    dbh("send_to_host",(char *)TxdBuf,i);

//    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
//    {
//          BSP_UartSend(SCOM1,TxdBuf,i); 
//    }
//    
//    xSemaphoreGive(gxMutex);

    send_to_host_queue(TxdBuf,i);


    return NO_ERR;
}


static SYSERRORCODE_E parseJSON(uint8_t *text,CMD_RX_T *cmd_rx)
{
    cJSON  *root = NULL;			                    // root    
    cJSON  *cmd = NULL;			                        // cmd     
    cJSON  *tmpJson = NULL;			                    // cmd
    SYSERRORCODE_E result = NO_ERR;
    
    uint8_t bcd_cmd[2] = {0};
    uint8_t bcd_dat[MAX_CMD_BCD_BUF_LEN] = {0};
    uint8_t asc_dat[2] = {0};
    uint16_t asc_len = 0;  
    
    root = cJSON_Parse((const char*)text);

    if (root == NULL)                 // ���ת�������򱨴��˳�
    {
        DBG("1.Error before: [%s]\r\n", cJSON_GetErrorPtr());
        cJSON_Delete(root);
        return CJSON_PARSE_ERR;
    }

//    DBG("json data = %s\r\n",text);

    //��ȡKEY,ָ������
//    cmd_rx->cmd_desc = (uint8_t *)cJSON_GetObjectItem(root,"cmd")->valuestring;  
    
    tmpJson = cJSON_GetObjectItem(root,"cmd");
    if(tmpJson == NULL)
    {
        DBG("2.Error before: [%s]\r\n", cJSON_GetErrorPtr());
        cJSON_Delete(root);
        return CJSON_GETITEM_ERR;
    }
    
    strcpy((char *)cmd_rx->cmd_desc,tmpJson->valuestring);  

//    DBG("cmd_rx->cmd_desc = %s\r\n",cmd_rx->cmd_desc);
    
    //��ȡָ����
    cmd = cJSON_GetObjectItem(root,"value"); 
    if(cmd == NULL)
    {
        DBG("3.Error before: [%s]\r\n", cJSON_GetErrorPtr());
          cJSON_Delete(root);
        return CJSON_GETITEM_ERR;
    } 
    
    tmpJson = cJSON_GetObjectItem(cmd,(const char*)cmd_rx->cmd_desc);
    
    if(tmpJson == NULL)
    {
        DBG("3.Error before: [%s]\r\n", cJSON_GetErrorPtr());
        cJSON_Delete(root);
        return CJSON_GETITEM_ERR;
    }      

    strcpy((char *)asc_dat,tmpJson->valuestring);

//    DBG("cmd len = %d,cmd = %s\r\n",strlen((const char*)asc_dat),asc_dat);
    
    asc2bcd(bcd_cmd, asc_dat, 2, 1); 

    //Ŀǰָ��ֻ��1byte ����ֱ�Ӹ�ֵ
    cmd_rx->cmd = bcd_cmd[0];

    //��ȡ����
    tmpJson = cJSON_GetObjectItem(root,"data");
    
    if(tmpJson != NULL)
    {
        asc_len = strlen((const char*)tmpJson->valuestring);

        //���������ݣ���ת��;�������򲻴���
        if(asc_len > 0)
        {
            if(asc_len % 2 != 0)
            {
                asc_len += 1;
            }
            
            if(cmd_rx->cmd == SET_MOTOR_A_PARAM || cmd_rx->cmd == SET_MOTOR_B_PARAM)
            {
                memcpy(cmd_rx->cmd_data,tmpJson->valuestring,asc_len);
                cmd_rx->len = strlen((const char*)tmpJson->valuestring);
            }
            else
            {
                asc2bcd(bcd_dat,(uint8_t *)tmpJson->valuestring,asc_len,1);
                memcpy(cmd_rx->cmd_data,bcd_dat,asc_len/2);
                cmd_rx->len = asc_len/2;
            }
        }

    }       



    cJSON_Delete(root);
    
    return result;

}



static uint16_t  packetJSON(CMD_TX_T *cmd_tx,uint8_t *command_data)
{
    char *TxdBuf;
    cJSON *root; // cJSONָ��
    uint16_t len = 0;//����json�ĳ���
    uint8_t strCmd[3] = {0};

    uint8_t tmp_code = cmd_tx->code;
    root=cJSON_CreateObject(); // ����root���󣬷���ֵΪcJSONָ��

    if (root == NULL)                 // ���ת�������򱨴��˳�
    {        
        cJSON_Delete(root);
        return 0;
    }


    sprintf(strCmd,"%02x",cmd_tx->cmd);
    
    cJSON_AddStringToObject(root,"cmd",strCmd);
    cJSON_AddNumberToObject(root,"code",tmp_code);
    cJSON_AddStringToObject(root,"data",(const char*)cmd_tx->data); 

    TxdBuf = cJSON_PrintUnformatted(root); 

    if(TxdBuf == NULL)
    {
        my_free(TxdBuf);
        cJSON_Delete(root);
        TxdBuf=NULL;  
        return 0;
    }

    strcpy((char *)command_data,TxdBuf);
    
    len = strlen((const char*)TxdBuf);

    cJSON_Delete(root);

    my_free(TxdBuf);
    
    TxdBuf=NULL;        

    return len;
}

static uint16_t  packetDeviceInfo(uint8_t *command_data)
{
    char *TxdBuf;
    cJSON *root,*dataobj; // cJSONָ��

    uint16_t len = 0;//����json�ĳ���

    root=cJSON_CreateObject(); // ����root���󣬷���ֵΪcJSONָ��
    dataobj=cJSON_CreateObject(); // ����dataobj���󣬷���ֵΪcJSONָ��

    if (root == NULL||dataobj == NULL)                 // ���ת�������򱨴��˳�
    {    
        my_free(TxdBuf);
        TxdBuf = NULL;        
        cJSON_Delete(root);
        return CJSON_CREATE_ERR;//ֱ�ӷ���һ���ϴ��ֵ���������鳤��
    }

    cJSON_AddStringToObject(root,"cmd","A3");
    cJSON_AddNumberToObject(root,"code",0);
    cJSON_AddItemToObject (root,"data",dataobj);
    
    cJSON_AddStringToObject(dataobj,"SW Version",(const char *)gDevinfo.SoftwareVersion);
    cJSON_AddStringToObject(dataobj,"HW Version",(const char *)gDevinfo.HardwareVersion);
    cJSON_AddStringToObject(dataobj,"BulidTime",(const char *)gDevinfo.BulidDate);
    cJSON_AddStringToObject(dataobj,"Model",(const char *)gDevinfo.Model);
    cJSON_AddStringToObject(dataobj,"ProductBatch",(const char *)gDevinfo.ProductBatch);
    cJSON_AddStringToObject(dataobj,"SN",(const char *)gDevinfo.GetSn());   

    
    TxdBuf = cJSON_PrintUnformatted(root); 

    if(TxdBuf == NULL)
    {    
        my_free(TxdBuf);
        TxdBuf = NULL;
        
        cJSON_Delete(root);
        return 0;
    }    

    strcpy((char *)command_data,TxdBuf);


//    DBG("send json data = %s\r\n",TxdBuf);

    len = strlen((const char*)TxdBuf);
    
    my_free(TxdBuf);
    cJSON_Delete(root);

    TxdBuf = NULL;

    return len;    
}


//static void displayTask(void)
//{
//    #ifdef DEBUG_PRINT
//    uint8_t pcWriteBuffer[1024];
//    printf("=================================================\r\n");
//    printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
//    vTaskList((char *)&pcWriteBuffer);
//    printf("%s\r\n", pcWriteBuffer);
//    
//    printf("\r\n������       ���м���         ʹ����\r\n");
//    vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//    printf("%s\r\n", pcWriteBuffer);    
//    #endif
//}


void send_to_device(CMD_RX_T *cmd_rx)
{
    uint16_t i = 0;
    uint8_t TxdBuf[JSON_PACK_MAX]={0};
    uint8_t tmpBuf[JSON_PACK_MAX] = {0};  
    
//    uint8_t setLed[39] = { 0x02,0x00,0x25,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x32,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x22,0x7d,0x03,0x85,0x7a };
    uint8_t setLed[39] = { 0x02,0x00,0x25,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x32,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x22,0x7d,0x03,0xA5,0xA5 };
    uint8_t retSensor[43] = { 0x02,0x00,0x29,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x31,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x30,0x30,0x30,0x30,0x22,0x7d,0x03,0xa5,0xa5 };
    
    uint8_t setLedValueStart[8] = { 0x01,0x06,0x08,0x0C,0x03,0x00,0x4B,0x59 };
    uint8_t setLedValueEnd[8] = { 0x01,0x06,0x08,0x0C,0x0A,0x00,0x4D,0x09 };
    
    uint8_t setLedValueStart2[8] = { 0x02,0x06,0x08,0x0C,0x03,0x00,0x4B,0x6A };
    uint8_t setLedValueEnd2[8] = { 0x02,0x06,0x08,0x0C,0x0A,0x00,0x4D,0x3A };    
    //uint16_t iCRC = 0;
    CMD_TX_T cmd_tx;
    
    MOTORCTRL_QUEUE_T *ptMotor = &gRecvMotorCtrlQueue; 
    MOTORCTRL_QUEUE_T *ptSecMotor = &gSecMotorCtrlQueue;  

    /* ���� */
    ptMotor->cmd = 0;
    ptMotor->len = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    
    /* ���� */
    ptSecMotor->cmd = 0;
    ptSecMotor->len = 0;
    memset(ptSecMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN);  
    
    memset(&cmd_tx,0x00,sizeof(CMD_TX_T));
    memset(TxdBuf,0x00,sizeof(TxdBuf));

	DBG("from android---> CMD = %02x,len = %d\r\n",cmd_rx->cmd,cmd_rx->len);
    dbh("from android--->", (char *)cmd_rx->cmd_data, cmd_rx->len);
    
    switch (cmd_rx->cmd)
    {
        case GETSENSOR://��ȡ����״̬��Ŀǰֻ���������

//            i = 3;
//            TxdBuf[0] = STX;            
//            cmd_tx.cmd = GETSENSOR;
//            cmd_tx.code = 0x00;
   
            i=43;
           	bsp_GetSensorValue(cmd_tx.data);
			memcpy(retSensor+32,cmd_tx.data,6);			
			memcpy(TxdBuf,retSensor,i);  
//		
//            i += packetJSON(&cmd_tx,tmpBuf);            
//            memcpy(TxdBuf+3,tmpBuf,i-3); 
//            TxdBuf[i++] = ETX;  
//            
//            TxdBuf[1] = i>>8; //high
//            TxdBuf[2] = i&0xFF; //low
//            
//       
//            TxdBuf[i++] = 0xA5;
//            TxdBuf[i++] = 0xA5;     

			//dbh("GETSENSOR", TxdBuf, i);
			
            break;
        case SETLED: //����LED��
            bsp_Ex_SetLed((uint8_t*)cmd_rx->cmd_data);             
            i = 39;
            memcpy(TxdBuf,setLed,i);   
            break;                        
        case GETDEVICEINFO://��ȡ�豸��Ϣ
            i = 3;
            TxdBuf[0] = STX; 
            i += packetDeviceInfo(tmpBuf); 
            memcpy(TxdBuf+3,tmpBuf,i-3); 
            TxdBuf[i++] = ETX;  
            
            TxdBuf[1] = i>>8; //high
            TxdBuf[2] = i&0xFF; //low
            
//            iCRC = CRC16_Modbus(TxdBuf, i);
//            TxdBuf[i++] = iCRC >> 8;
//            TxdBuf[i++] = iCRC & 0xff;    
            TxdBuf[i++] = 0xA5;
            TxdBuf[i++] = 0xA5;              

//            dbh("send_to_GETDEVICEINFO",(char *)TxdBuf,i);
            break;           
      
        case GETVER:  //��ȡ����汾��
            i = 3;
            TxdBuf[0] = STX;
            cmd_tx.cmd = GETVER;
            cmd_tx.code = 0x00;
            strcpy((char *)cmd_tx.data,(const char*)gDevinfo.SoftwareVersion);            
            i += packetJSON(&cmd_tx,tmpBuf); 
            memcpy(TxdBuf+3,tmpBuf,i-3); 
            TxdBuf[i++] = ETX;     

            TxdBuf[1] = i>>8; //high
            TxdBuf[2] = i&0xFF; //low
            
//            iCRC = CRC16_Modbus(TxdBuf, i);  
//            TxdBuf[i++] = iCRC >> 8;
//            TxdBuf[i++] = iCRC & 0xff;   
            TxdBuf[i++] = 0xA5;
            TxdBuf[i++] = 0xA5;              

            break; 
            
        case SETDEVPARAM://�����豸����
            ParseDevParam(cmd_rx->cmd_data);//����������д��FLASH
            i = 3;
            TxdBuf[0] = STX;
            cmd_tx.cmd = SETDEVPARAM;
            cmd_tx.code = 0x00;
            strcpy((char *)cmd_tx.data,"00");      
            i += packetJSON(&cmd_tx,tmpBuf); 
            memcpy(TxdBuf+3,tmpBuf,i-3); 
            TxdBuf[i++] = ETX;     

            TxdBuf[1] = i>>8; //high
            TxdBuf[2] = i&0xFF; //low
            
//            iCRC = CRC16_Modbus(TxdBuf, i);  
//            TxdBuf[i++] = iCRC >> 8;
//            TxdBuf[i++] = iCRC & 0xff;   
            TxdBuf[i++] = 0xA5;
            TxdBuf[i++] = 0xA5;              

            break; 

        case DEVRESET:
            NVIC_SystemReset();
            break;
        case UPGRADE:
            SystemUpdate();
            break; 
            
        case CONTROLMOTOR_A:
             //�������Ϳ���ָ��
//            ptMotor->cmd = CONTROLMOTOR_A;
//            ptMotor->len = cmd_rx->len;
//            memcpy(ptMotor->data,cmd_rx->cmd_data,cmd_rx->len); 

//			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
//			if(xQueueSend(gxMotorCtrlQueue,             /* ��Ϣ���о�� */
//						 (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
//						 (TickType_t)50) != pdPASS )
//			{
//                xQueueReset(gxMotorCtrlQueue);
//                DBG("A the queue is error!\r\n"); 
//            } 
            packetToMotorA(CONTROLMOTOR_A,cmd_rx->cmd_data,cmd_rx->len);
            
            return;//���ﲻ��Ҫ����λ�����ͣ�������һ�������в�����
        case CONTROLMOTOR_B:
             //�������Ϳ���ָ��
            ptSecMotor->cmd = CONTROLMOTOR_B;
            ptSecMotor->len = cmd_rx->len;
            memcpy(ptSecMotor->data,cmd_rx->cmd_data,cmd_rx->len); 

			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
			if(xQueueSend(gxMotorSecDoorCtrlQueue,      /* ��Ϣ���о�� */
						 (void *) &ptSecMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
						 (TickType_t)50) != pdPASS )
			{
                xQueueReset(gxMotorSecDoorCtrlQueue);
                DBG("B the queue is error!\r\n"); 
//                DBG("B the queue is full!\r\n");                             
            } 
//            else
//            {
//                DBG("B the queue is success!\r\n"); 
//            }
            
            return;//���ﲻ��Ҫ����λ�����ͣ�������һ�������в�����

       case SET_MOTOR_A_PARAM:
//            DBG("SET_MOTOR_A_PARAM!  cmd_rx = %s\r\n",cmd_rx->cmd_data);
            parseMotorParam(cmd_rx);  
            return;

       case SET_MOTOR_B_PARAM:
//            DBG("SET_MOTOR_B_PARAM!\r\n");
            parseMotorParam(cmd_rx);  
            return;

       case SHOWINFORMATION_STATIC:
            SetLedShowInfo(cmd_rx,SHOWINFORMATION_STATIC);
            return;

       case SHOWINFORMATION:
            SetLedShowInfo(cmd_rx,SHOWINFORMATION);
            return;

       case SETLEDVALUE: 
            ptMotor->cmd = SETLEDVALUE;
            ptMotor->len = 8;
            memcpy(ptMotor->data,setLedValueStart,8); 

			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
			if(xQueueSend(gxMotorCtrlQueue,             /* ��Ϣ���о�� */
						 (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
						 (TickType_t)50) != pdPASS )
			{
                xQueueReset(gxMotorCtrlQueue);
                DBG("A the queue is error!\r\n"); 
            } 

            
             //�������Ϳ���ָ��
            packetToMotorA(SETLEDVALUE,setLedValueStart2,8);
//            packetToMotorA(SETLEDVALUE,setLedValueStart2,8);
            return;            
       case SETLEDVALUE_END: 

            ptMotor->cmd = SETLEDVALUE_END;
            ptMotor->len = 8;
            memcpy(ptMotor->data,setLedValueEnd,8); 

			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
			if(xQueueSend(gxMotorCtrlQueue,             /* ��Ϣ���о�� */
						 (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
						 (TickType_t)50) != pdPASS )
			{
                xQueueReset(gxMotorCtrlQueue);
                DBG("A the queue is error!\r\n"); 
            } 

            
             //�������Ϳ���ָ��
//            packetToMotorA(SETLEDVALUE_END,setLedValueEnd,8);
            packetToMotorA(SETLEDVALUE_END,setLedValueEnd2,8);            
            return;                        
        default:
            init_serial_boot(); 
            return;
    }


    send_to_host_queue(TxdBuf,i);



}


static void parseMotorParam(CMD_RX_T *cmd_rx)
{
    int i = 0;
    uint16_t iCRC = 0;
    //�ָ�����ַ����ĸ���
    int num = 0;
    //�������շ������ݵ����顣���������Ԫ��ֻҪ���õıȷָ������ַ���������ͺ��ˡ�
    char *revbuf[6] = {0}; //��ŷָ������ַ��� 
    uint8_t TxdBuf[8+1] = {0};


    MOTORCTRL_QUEUE_T *ptMotor; 

    /* ��ʼ���ṹ��ָ�� */
    ptMotor = &gMotorCtrlQueue;

    /* ���� */
    ptMotor->cmd = 0;
    ptMotor->len = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 	  

    if(cmd_rx == NULL)
    {
        return ;
    }
      
    split((char *)cmd_rx->cmd_data,",",revbuf,&num); //���ú������зָ�
    
    //������ص�ÿ������ 
    for(i = 0;i < num; i++) 
    {
        TxdBuf[0] = 0x00;   
        TxdBuf[1] = 0x06;
        TxdBuf[2] = 0x09;   
        TxdBuf[3] = i;   
        TxdBuf[4] = atoi(revbuf[i])>>8; //high
        TxdBuf[5] = atoi(revbuf[i])&0xFF; //low
                
        iCRC = CRC16_Modbus(TxdBuf, 6);  
        TxdBuf[6] = iCRC & 0xff;//��8λ
        TxdBuf[7] = iCRC >> 8;  //��8λ

       

        ptMotor->cmd = cmd_rx->cmd;
        
        memcpy(ptMotor->data,TxdBuf,ptMotor->len); 

        if(ptMotor->cmd == SET_MOTOR_A_PARAM)
        {
            /* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
            if(xQueueSend(gxMotorCtrlQueue,      /* ��Ϣ���о�� */
            			 (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
            			 (TickType_t)100) != pdPASS )
            {
                xQueueReset(gxMotorCtrlQueue);
            }
        }
        else if(ptMotor->cmd == SET_MOTOR_B_PARAM)
        {

            /* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
            if(xQueueSend(gxMotorSecDoorCtrlQueue,      /* ��Ϣ���о�� */
                         (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
                         (TickType_t)100) != pdPASS )
            {
                xQueueReset(gxMotorSecDoorCtrlQueue);
            }
        }

        vTaskDelay(100);
    }
}

static void SetLedShowInfo(CMD_RX_T *cmd_rx,char mode)
{
    uint8_t tmpBuf[MAX_HOST_CMD_LEN] = {0};
    uint16_t i = 0;
    uint8_t times = 6;

    if(cmd_rx == NULL)
    {
        return ;
    }

    memset(tmpBuf,0x00,sizeof(tmpBuf));

    tmpBuf[i++] = 0x02;
    tmpBuf[i++] = 0x21;
    tmpBuf[i++] = 0x25;
    tmpBuf[i++] = 0x20;    
    tmpBuf[i++] = 0x0C;   
    
    tmpBuf[i++] = mode;  
    tmpBuf[i++] = 0x20;  
    if(mode == 0x20)
    {
        tmpBuf[i++] = 0xFF;
    }
    else
    {
        tmpBuf[i++] = 0x20;
    }
      
    tmpBuf[i++] = 0x20;  

    tmpBuf[i++] = 0x20 + cmd_rx->len;  
    
    memcpy(tmpBuf+i,cmd_rx->cmd_data,cmd_rx->len);
    
    tmpBuf[i+cmd_rx->len] = 0x03;

    dbh("SetLedShowInfo", (char *)tmpBuf, i+cmd_rx->len+1);

    while (times--)
    {   
        RS485_SendBuf(COM6,tmpBuf,i+cmd_rx->len+1); 
        vTaskDelay(200);  
    }

        
}


SYSERRORCODE_E SendAsciiCodeToHost(uint8_t cmd,SYSERRORCODE_E code,uint8_t *buf)
{
    SYSERRORCODE_E result = NO_ERR;

    uint16_t i = 0;
    uint16_t json_len = 0;
    uint8_t TxdBuf[JSON_PACK_MAX]={0};
    uint8_t tmpBuf[MAX_TXD_BUF_LEN] = {0};
    //uint16_t iCRC = 0;
    CMD_TX_T cmd_tx;

    memset(tmpBuf,0x00,sizeof(tmpBuf));
    memset(TxdBuf,0x00,sizeof(TxdBuf));
    memset(&cmd_tx,0x00,sizeof(cmd_tx));

    i = 3;
    TxdBuf[0] = STX;
    cmd_tx.cmd = cmd;
    cmd_tx.code = code;    
    strcpy((char *)cmd_tx.data,(char *)buf);    

    json_len = packetJSON(&cmd_tx,tmpBuf);  

    //DBG("json packet = %s,len = %d\r\n",tmpBuf,json_len);

    if(json_len == 0)
    {        
        return CJSON_PACKET_ERR;
    }
    else
    {
        i += json_len;
    }
    
    memcpy(TxdBuf+3,tmpBuf,i-3); 
    TxdBuf[i++] = ETX;   

    TxdBuf[1] = i>>8; //high
    TxdBuf[2] = i&0xFF; //low    

//    //2020.11.09 �޸ģ���Ϊ��λ�������飬����У��ֵ�п��ܸ���ͷ��ͻ����������Ϊ�̶�ֵ
//    iCRC = CRC16_Modbus(TxdBuf, i);  
//    TxdBuf[i++] = iCRC >> 8;
//    TxdBuf[i++] = iCRC & 0xff;  

    TxdBuf[i++] = 0xA5;
    TxdBuf[i++] = 0xA5;    


//    dbh("SendAsciiCodeToHost", (char *)TxdBuf, i);

//    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
//    {
//        BSP_UartSend(SCOM1,TxdBuf,i); 
//    }    
//    xSemaphoreGive(gxMutex);

    send_to_host_queue(TxdBuf,i);
    return result;
}


void respondLed(void)
{    
//    uint8_t retLed[39] = { 0x02,0x00,0x25,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x32,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x22,0x7d,0x03,0x85,0x7a };
    uint8_t retLed[39] = { 0x02,0x00,0x25,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x32,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x22,0x7d,0x03,0xA5,0xA5 };


//    DBG("SEND LED RESPOND\r\n\r\n");
    
//    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
//    {
//        BSP_UartSend(SCOM1,retLed,39); 
//    }
//    
//    xSemaphoreGive(gxMutex); 
    send_to_host_queue(retLed,39);

}


void KeyOpenDoorB(void)
{
//    uint8_t open[61] = { 0x02,0x00,0x3b,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x62,0x37,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x52,0x65,0x71,0x75,0x65,0x73,0x74,0x20,0x74,0x6f,0x20,0x6f,0x70,0x65,0x6e,0x20,0x74,0x68,0x65,0x20,0x64,0x6f,0x6f,0x72,0x22,0x7d,0x03,0xa4,0x87 };
    uint8_t open[61] = { 0x02,0x00,0x3b,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x62,0x37,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x52,0x65,0x71,0x75,0x65,0x73,0x74,0x20,0x74,0x6f,0x20,0x6f,0x70,0x65,0x6e,0x20,0x74,0x68,0x65,0x20,0x64,0x6f,0x6f,0x72,0x22,0x7d,0x03,0xA5,0xA5 };
    DBG("KeyOpenDoorB\r\n\r\n");
    
    
//    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
//    {
//        BSP_UartSend(SCOM1,open,61); 
//    }
//    
//    xSemaphoreGive(gxMutex); 

    send_to_host_queue(open,61);
}

//void respHost(uint8_t *cmd,uint8_t len)
//{
//    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
//    {
//        BSP_UartSend(SCOM1,cmd,len); 
//    }
//    
//    xSemaphoreGive(gxMutex); 
//}

char send_to_host_queue(uint8_t *buf,int len)
{
    TOHOST_QUEUE_T *ptMsg = &gToHostQueueBuf;   

    if(buf ==NULL || len == 0)
    {
        return COMM_PARSE_ERR;
    }

    //dbh("0 send_to_host_queue", (char *)buf, len);

    ptMsg->len = len; 
    memset(ptMsg->data,0x00,sizeof(ptMsg->data)); 
    
    memcpy(ptMsg->data,buf,len);

    //DBG("execute %d times\r\n",count++);
    
    //dbh("1 send_to_host_queue",  (char *)ptMsg->data, ptMsg->len);


    
    /* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
    if(xQueueSend(gxToHostQueue,                /* ��Ϣ���о�� */
                 (void *) &ptMsg,               /* ����ָ�����recv_buf�ĵ�ַ */
                 (TickType_t)20) != pdPASS )
    {              
        xQueueReset(gxToHostQueue);      
        DBG("queue is full\r\n");
    } 
    else
    {       
        
        //dbh("2 send_to_host_queue",  (char *)ptMsg->data, ptMsg->len);
    }

    return NO_ERR;    
}



static void packetToMotorA(uint8_t cmd,uint8_t *buff,uint8_t len)
{
    MOTORCTRL_QUEUE_T *ptMotor = &gMotorCtrlQueue; 
  
    ptMotor->len = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN);     

    ptMotor->cmd = cmd;
    ptMotor->len = len;
    memcpy(ptMotor->data,buff,len); 

	/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
	if(xQueueSend(gxMotorCtrlQueue,             /* ��Ϣ���о�� */
				 (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
				 (TickType_t)20) != pdPASS )
	{
        xQueueReset(gxMotorCtrlQueue);
        DBG("A the queue is error!\r\n"); 
    }     
}


