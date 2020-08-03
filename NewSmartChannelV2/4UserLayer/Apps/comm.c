/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : comm.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月18日
  最近修改   :
  功能描述   : 解析串口指令
  函数列表   :
  修改历史   :
  1.日    期   : 2019年6月18日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
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



/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

MOTORCTRL_QUEUE_T gMotorCtrlQueue;    //定义一个结构体用于消息队列，用于同步处理相应数据
MOTORCTRL_QUEUE_T gSecMotorCtrlQueue;    //定义一个结构体用于消息队列，用于同步处理相应数据



RECVHOST_T gRecvHost;
static uint16_t crc_value = 0;


static SYSERRORCODE_E parseJSON(uint8_t *text,CMD_RX_T *cmd_rx); //私有函数
static uint16_t  packetJSON(CMD_TX_T *cmd_tx,uint8_t *command_data);
static uint16_t  packetDeviceInfo(uint8_t *command_data);
static void parseMotorParam(CMD_RX_T *cmd_rx);



//static void displayTask(void);



/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

    
void init_serial_boot(void)
{
    crc_value = 0;    
    memset(&gRecvHost,0x00,sizeof(gRecvHost));
}

/*****************************************************************************
 函 数 名  : deal_Serial_Parse
 功能描述  :     将收到的数据解析成数据包；
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月18日
    作    者   : 张舵
    修改内容   : 新生成函数

*****************************************************************************/
// 7E 01 A1 00 AA 74 
// 7E 01 A1 AA 25 48 00 AA 74 7E 01 A1 00 AA 74


void deal_Serial_Parse(void)
{
    uint8_t ch = 0; 
    
    while(1)
    {  
//        if(bsp_Usart1_RecvOne(&ch) != 1)  //读取串口数据
//        if(comGetChar(COM1, &ch) != 1)    //读取串口数据

        if(BSP_UartRead(SCOM1,&ch,1)!=1)
        {
            delay_ms(10);
            
            return;
        }

        
        switch (gRecvHost.RxdStatus)
        { /*接收数据状态*/                
            case 0:
                gRecvHost.RxdStatus = SPACE;
                if(STX == ch) /*接收包头*/
                {
                    gRecvHost.RxdBuf[0] = ch;
                    gRecvHost.NowLen = 1;
                    gRecvHost.RxdStatus = 20;
                }
                break;              
            case 20:      /* 接收整个数据包 */
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
                    
//                    dbh("recv finish",(char *)gRecvHost.RxdBuf,gRecvHost.RxdTotalLen);
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
 函 数 名  : deal_rx_data
 功能描述  : 解析收到的完整数据包，并响应数据包；
 输入参数  : void
 输出参数  : 无
 返 回 值  : void
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2019年6月18日
    作    者   : 张舵
    修改内容   : 新生成函数

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
        if(gRecvHost.RxdBuf[0] == STX && gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -3] == ETX)                                   //解析02数据包
        {    
//            dbh("recv deal_rx_data",(char *)gRecvHost.RxdBuf,gRecvHost.RxdTotalLen);
            
            //计算校验值
            bccHi =  crc_value >> 8;
            bccLo = crc_value & 0xff;
            
            if ((bccHi == gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -2]) && \
                (bccLo == gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -1]))
            {
				//转存JSON数据包
                memcpy(json_buf,(const char*)gRecvHost.RxdBuf+1,gRecvHost.RxdTotalLen-4);
//                DBG("recv json data = : %s\r\n",json_buf);

                //解析JSON数据包              
                if(parseJSON(json_buf,&cmd_rx) == NO_ERR)
                {
                    send_to_device(&cmd_rx);
                }
                else
                {
                    //指令解析失败，向上位机发送解析失败的状态，要求重发
                     DBG("parseJSON error\r\n");
                    SendAsciiCodeToHost(ERRORINFO,COMM_PARSE_ERR,"cmd parse error");
                }

                init_serial_boot();   


            }
            else
            {
                DBG("CRC ERROR\r\n");
                dbh("CRC ERROR RxdBuf", (char *)gRecvHost.RxdBuf, gRecvHost.RxdTotalLen);
//                DBG("bccHi = %02x,bccLo = %02x",bccHi,bccLo);
                SendAsciiCodeToHost(ERRORINFO,COMM_CRC_ERR,"deal rx data crc error");

                init_serial_boot();
              
            }
        }
        else
        {
            DBG("-----------execute deal_rx_data-----------\r\n");
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
    uint16_t iCRC = 0;
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
            
    iCRC = CRC16_Modbus(TxdBuf, i);  
    TxdBuf[i++] = iCRC >> 8;
    TxdBuf[i++] = iCRC & 0xff;  

//    DBG("send json data = %s\r\n",tmpBuf);
//    dbh("send_to_host",(char *)TxdBuf,i);

    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
    {
          BSP_UartSend(SCOM1,TxdBuf,i); 
    }
    
    xSemaphoreGive(gxMutex);

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
    uint8_t asc_dat[MAX_CMD_BUF_LEN] = {0};
    uint16_t asc_len = 0;  
    
    root = cJSON_Parse((const char*)text);

    if (root == NULL)                 // 如果转化错误，则报错退出
    {
        DBG("1.Error before: [%s]\n", cJSON_GetErrorPtr());
          cJSON_Delete(root);
        return CJSON_PARSE_ERR;
    }

    //获取KEY,指令描述
//    cmd_rx->cmd_desc = (uint8_t *)cJSON_GetObjectItem(root,"cmd")->valuestring;  
    
    tmpJson = cJSON_GetObjectItem(root,"cmd");
    if(tmpJson == NULL)
    {
        DBG("2.Error before: [%s]\n", cJSON_GetErrorPtr());
        cJSON_Delete(root);
        return CJSON_GETITEM_ERR;
    }
    
    strcpy(cmd_rx->cmd_desc,tmpJson->valuestring);  

    DBG("cmd_rx->cmd_desc = %s\r\n",cmd_rx->cmd_desc);
    
    //获取指令字
    cmd = cJSON_GetObjectItem(root,"value"); 
    if(cmd == NULL)
    {
        DBG("3.Error before: [%s]\n", cJSON_GetErrorPtr());
          cJSON_Delete(root);
        return CJSON_GETITEM_ERR;
    } 
    
    tmpJson = cJSON_GetObjectItem(cmd,(const char*)cmd_rx->cmd_desc);
    
    if(tmpJson == NULL)
    {
        DBG("3.Error before: [%s]\n", cJSON_GetErrorPtr());
        cJSON_Delete(root);
        return CJSON_GETITEM_ERR;
    }      

    strcpy((char *)asc_dat,tmpJson->valuestring);

    DBG("cmd len = %d,cmd = %s\r\n",strlen((const char*)asc_dat),asc_dat);
    
    asc2bcd(bcd_cmd, asc_dat, 2, 1); 

    //目前指令只有1byte 所以直接赋值
    cmd_rx->cmd = bcd_cmd[0];

    //获取数据
    tmpJson = cJSON_GetObjectItem(root,"data");
    
    if(tmpJson == NULL)
    {
        DBG("3.Error before: [%s]\n", cJSON_GetErrorPtr());
          cJSON_Delete(root);
        return CJSON_GETITEM_ERR;
    }       

    asc_len = strlen((const char*)tmpJson->valuestring);

    //若是有数据，则转换;无数据则不处理
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
            asc2bcd(bcd_dat,tmpJson->valuestring,asc_len,1);
            memcpy(cmd_rx->cmd_data,bcd_dat,asc_len/2);
            cmd_rx->len = asc_len/2;
        }
    }

    cJSON_Delete(root);
    
    return result;

}



static uint16_t  packetJSON(CMD_TX_T *cmd_tx,uint8_t *command_data)
{
    char *TxdBuf;
    cJSON *root; // cJSON指针
    uint16_t len = 0;//返回json的长度

    uint8_t tmp_code = cmd_tx->code;
//    uint8_t tmpCmddata[MAX_CMD_BUF_LEN] = {0}; 
    
//    memset(tmpCmddata,0x00,sizeof(tmpCmddata));
    root=cJSON_CreateObject(); // 创建root对象，返回值为cJSON指针

    if (root == NULL)                 // 如果转化错误，则报错退出
    {
        cJSON_Delete(root);
        return 0;
    }

    //这里若是不中转则，JSON打包的数据会变，原因未知  2019.07.12 surge
//    memcpy(tmpCmddata,cmd_tx->data,strlen((const char*)cmd_tx->data));
    
    sprintf(TxdBuf,"%02x",cmd_tx->cmd);
    
    cJSON_AddStringToObject(root,"cmd",TxdBuf);
    cJSON_AddNumberToObject(root,"code",tmp_code);
//    cJSON_AddStringToObject(root,"data",(const char*)tmpCmddata);   
    cJSON_AddStringToObject(root,"data",(const char*)cmd_tx->data);   
    

    TxdBuf = cJSON_PrintUnformatted(root); 

    if(TxdBuf == NULL)
    {
        cJSON_Delete(root);
        return 0;
    }

    strcpy((char *)command_data,TxdBuf);
    
    len = strlen((const char*)TxdBuf);

    cJSON_Delete(root);

    my_free(TxdBuf);

    return len;
}

static uint16_t  packetDeviceInfo(uint8_t *command_data)
{
    char *TxdBuf;
    cJSON *root,*dataobj; // cJSON指针

    uint16_t len = 0;//返回json的长度

    root=cJSON_CreateObject(); // 创建root对象，返回值为cJSON指针
    dataobj=cJSON_CreateObject(); // 创建dataobj对象，返回值为cJSON指针

    if (root == NULL||dataobj == NULL)                 // 如果转化错误，则报错退出
    {
        cJSON_Delete(root);
        return CJSON_CREATE_ERR;//直接返回一个较大的值，超出数组长度
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
        cJSON_Delete(root);
        return 0;
    }    

    strcpy((char *)command_data,TxdBuf);


    DBG("send json data = %s\r\n",TxdBuf);

    len = strlen((const char*)TxdBuf);

    cJSON_Delete(root);

    my_free(dataobj);
    my_free(TxdBuf);

    return len;    
}


//static void displayTask(void)
//{
//    #ifdef DEBUG_PRINT
//    uint8_t pcWriteBuffer[1024];
//    printf("=================================================\r\n");
//    printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
//    vTaskList((char *)&pcWriteBuffer);
//    printf("%s\r\n", pcWriteBuffer);
//    
//    printf("\r\n任务名       运行计数         使用率\r\n");
//    vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//    printf("%s\r\n", pcWriteBuffer);    
//    #endif
//}


void send_to_device(CMD_RX_T *cmd_rx)
{
    BaseType_t xReturn = pdPASS;//定义一个创建信息返回值，默认为pdPASS
    uint16_t i = 0;
    uint8_t TxdBuf[JSON_PACK_MAX]={0};
    uint8_t tmpBuf[JSON_PACK_MAX] = {0};  
    uint16_t iCRC = 0;
    CMD_TX_T cmd_tx;
    
    MOTORCTRL_QUEUE_T *ptMotor; 
    MOTORCTRL_QUEUE_T *ptSecMotor; 
    
	/* 初始化结构体指针 */
	ptMotor = &gMotorCtrlQueue;

    /* 清零 */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 	

	/* 初始化结构体指针 */
	ptSecMotor = &gSecMotorCtrlQueue;
    /* 清零 */
    ptSecMotor->cmd = 0;
    memset(ptSecMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN);     
    
    memset(&cmd_tx,0x00,sizeof(CMD_TX_T));
    memset(TxdBuf,0x00,sizeof(TxdBuf));
    
    switch (cmd_rx->cmd)
    {
        case GETSENSOR://获取红外状态，目前只有两组红外
//            displayTask();
            i = 3;
            TxdBuf[0] = STX;            
            cmd_tx.cmd = GETSENSOR;
            cmd_tx.code = 0x00;
            //bsp_GetSensorStatus(cmd_tx.data);      
            bsp_GetSensorValue(cmd_tx.data);
            i += packetJSON(&cmd_tx,tmpBuf);            
            memcpy(TxdBuf+3,tmpBuf,i-3); 
            TxdBuf[i++] = ETX;  
            
            TxdBuf[1] = i>>8; //high
            TxdBuf[2] = i&0xFF; //low
            
            iCRC = CRC16_Modbus(TxdBuf, i);  
            
            TxdBuf[i++] = iCRC >> 8;
            TxdBuf[i++] = iCRC & 0xff;            
            break;
        case SETLED: //设置LED灯
          xReturn = xTaskNotify( xHandleTaskLed, /*任务句柄*/
                                 (uint32_t)&cmd_rx->cmd_data,
                                 eSetValueWithOverwrite );/*覆盖当前通知*/
          
            if( xReturn != pdPASS )
            {
                SendAsciiCodeToHost(ERRORINFO,COMM_SEND_ERR,"set led error,try again");
                DBG("Set LED Send Error!\r\n");
                dbh("Set LED Send Error", (char *)cmd_rx->cmd_data, MAX_EXLED_LEN);                
            }
            
            break;                        
        case GETDEVICEINFO://获取设备信息
            i = 3;
            TxdBuf[0] = STX; 
            i += packetDeviceInfo(tmpBuf); 
            memcpy(TxdBuf+3,tmpBuf,i-3); 
            TxdBuf[i++] = ETX;  
            
            TxdBuf[1] = i>>8; //high
            TxdBuf[2] = i&0xFF; //low
            
            iCRC = CRC16_Modbus(TxdBuf, i);  
            
            TxdBuf[i++] = iCRC >> 8;
            TxdBuf[i++] = iCRC & 0xff;    

//            dbh("send_to_GETDEVICEINFO",(char *)TxdBuf,i);
            break;           
      
        case GETVER:  //获取软件版本号
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
            
            iCRC = CRC16_Modbus(TxdBuf, i);  
            TxdBuf[i++] = iCRC >> 8;
            TxdBuf[i++] = iCRC & 0xff;   
            break; 
            
        case SETDEVPARAM://设置设备参数
            ParseDevParam(cmd_rx->cmd_data);//解析参数并写入FLASH
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
            
            iCRC = CRC16_Modbus(TxdBuf, i);  
            TxdBuf[i++] = iCRC >> 8;
            TxdBuf[i++] = iCRC & 0xff;   
            break; 

        case DEVRESET:
            NVIC_SystemReset();
            break;
        case UPGRADE:
            SystemUpdate();
            break;         

        case CONTROLMOTOR_A:
             //向电机发送控制指令
            ptMotor->cmd = CONTROLMOTOR_A;
            memcpy(ptMotor->data,cmd_rx->cmd_data,MOTORCTRL_QUEUE_BUF_LEN); 

			/* 使用消息队列实现指针变量的传递 */
			if(xQueueSend(gxMotorCtrlQueue,             /* 消息队列句柄 */
						 (void *) &ptMotor,             /* 发送结构体指针变量ptReader的地址 */
						 (TickType_t)100) != pdPASS )
			{
                xQueueReset(gxMotorCtrlQueue);
            } 
//            else
//            {
//                DBG("A the queue is success!\r\n"); 
//            }
            
            return;//这里不需要向上位机上送，在另外一个任务中才上送
        case CONTROLMOTOR_B:
             //向电机发送控制指令
            ptSecMotor->cmd = CONTROLMOTOR_B;
            memcpy(ptSecMotor->data,cmd_rx->cmd_data,MOTORCTRL_QUEUE_BUF_LEN); 

			/* 使用消息队列实现指针变量的传递 */
			if(xQueueSend(gxMotorSecDoorCtrlQueue,      /* 消息队列句柄 */
						 (void *) &ptSecMotor,             /* 发送结构体指针变量ptReader的地址 */
						 (TickType_t)100) != pdPASS )
			{
                xQueueReset(gxMotorSecDoorCtrlQueue);
//                DBG("B the queue is full!\r\n");                             
            } 
//            else
//            {
//                DBG("B the queue is success!\r\n"); 
//            }
            
            return;//这里不需要向上位机上送，在另外一个任务中才上送

       case SET_MOTOR_A_PARAM:
            DBG("SET_MOTOR_A_PARAM!  cmd_rx = %s\r\n",cmd_rx->cmd_data);
            parseMotorParam(cmd_rx);  
            return;

       case SET_MOTOR_B_PARAM:
            DBG("SET_MOTOR_B_PARAM!\r\n");
            parseMotorParam(cmd_rx);  
            return;

            
        default:
            init_serial_boot(); 
            return;
    }


    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
    {
        BSP_UartSend(SCOM1,TxdBuf,i); 
    }
    xSemaphoreGive(gxMutex);

}


static void parseMotorParam(CMD_RX_T *cmd_rx)
{
    int i = 0;
    uint16_t iCRC = 0;
    //分割后子字符串的个数
    int num = 0;
    //用来接收返回数据的数组。这里的数组元素只要设置的比分割后的子字符串个数大就好了。
    char *revbuf[6] = {0}; //存放分割后的子字符串 
    uint8_t TxdBuf[8+1] = {0};


    MOTORCTRL_QUEUE_T *ptMotor; 

    /* 初始化结构体指针 */
    ptMotor = &gMotorCtrlQueue;

    /* 清零 */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 	  

    if(cmd_rx == NULL)
    {
        return ;
    }
      
    split((char *)cmd_rx->cmd_data,",",revbuf,&num); //调用函数进行分割
    
    //输出返回的每个内容 
    for(i = 0;i < num; i++) 
    {
        TxdBuf[0] = 0x01;   
        TxdBuf[1] = 0x06;
        TxdBuf[2] = 0x09;   
        TxdBuf[3] = i;   
        TxdBuf[4] = atoi(revbuf[i])>>8; //high
        TxdBuf[5] = atoi(revbuf[i])&0xFF; //low
                
        iCRC = CRC16_Modbus(TxdBuf, 6);  
        TxdBuf[6] = iCRC & 0xff;//低8位
        TxdBuf[7] = iCRC >> 8;  //高8位

       

        ptMotor->cmd = cmd_rx->cmd;
        
        memcpy(ptMotor->data,TxdBuf,MOTORCTRL_QUEUE_BUF_LEN); 

        if(ptMotor->cmd == SET_MOTOR_A_PARAM)
        {
            /* 使用消息队列实现指针变量的传递 */
            if(xQueueSend(gxMotorCtrlQueue,      /* 消息队列句柄 */
            			 (void *) &ptMotor,             /* 发送结构体指针变量ptReader的地址 */
            			 (TickType_t)100) != pdPASS )
            {
                xQueueReset(gxMotorCtrlQueue);
            }
        }
        else if(ptMotor->cmd == SET_MOTOR_B_PARAM)
        {

            /* 使用消息队列实现指针变量的传递 */
            if(xQueueSend(gxMotorSecDoorCtrlQueue,      /* 消息队列句柄 */
                         (void *) &ptMotor,             /* 发送结构体指针变量ptReader的地址 */
                         (TickType_t)100) != pdPASS )
            {
                xQueueReset(gxMotorSecDoorCtrlQueue);
            }
        }

        vTaskDelay(60);
    }
}


SYSERRORCODE_E SendAsciiCodeToHost(uint8_t cmd,SYSERRORCODE_E code,uint8_t *buf)
{
    SYSERRORCODE_E result = NO_ERR;

    uint16_t i = 0;
    uint16_t json_len = 0;
    uint8_t TxdBuf[JSON_PACK_MAX]={0};
    uint8_t tmpBuf[MAX_TXD_BUF_LEN] = {0};
    uint16_t iCRC = 0;
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
            
    iCRC = CRC16_Modbus(TxdBuf, i);  
    TxdBuf[i++] = iCRC >> 8;
    TxdBuf[i++] = iCRC & 0xff;  



    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
    {
        BSP_UartSend(SCOM1,TxdBuf,i); 
    }
    
    xSemaphoreGive(gxMutex);
    
    return result;
}



