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
#include "tool.h"
#include "bsp_led.h"
#include "malloc.h"
#include "ini.h"
#include "ymodem.h"
#include "bsp_uart_fifo.h"



/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/


SemaphoreHandle_t  gxMutex = NULL;


RECVHOST_T gRecvHost;


static uint16_t crc_value = 0;


static SYSERRORCODE_E parseJSON(uint8_t *text,CMD_RX_T *cmd_rx); //私有函数
static uint8_t  packetJSON(CMD_TX_T *cmd_tx,uint8_t *command_data);



/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

    
void init_serial_boot(void)
{
    comClearRxFifo(COM1);
    comClearRxFifo(COM1);
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
        #if CMD_SERIAL_PORT == 0x01
//        if(bsp_Usart1_RecvOne(&ch) != 1)  //读取串口数据
        if(comGetChar(COM1, &ch) != 1)  //读取串口数据
        {
            delay_ms(10);
            
            return;
        }
        #else
        if(bsp_Usart2_RecvOne(&ch) != 1)  //读取串口数据
        {
            delay_ms(10);
            
            return;
        }
        #endif
        
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
                    DBG("\r\n\r\n<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>\r\n\r\n");
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

    memset(&cmd_rx,0x00,sizeof(cmd_rx));  
    
    if (FINISH == gRecvHost.RxdFrameStatus)
    {        
        if(gRecvHost.RxdBuf[0] == STX && gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -3] == ETX)                                   //解析02数据包
        {    
            dbh("recv deal_rx_data",(char *)gRecvHost.RxdBuf,gRecvHost.RxdTotalLen);
            
            //计算校验值
            bccHi =  crc_value >> 8;
            bccLo = crc_value & 0xff;
            
            if ((bccHi == gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -2]) && \
                (bccLo == gRecvHost.RxdBuf[gRecvHost.RxdTotalLen -1]))
            {
				//转存JSON数据包
                memcpy(json_buf,(const char*)gRecvHost.RxdBuf+1,gRecvHost.RxdTotalLen-4);
                DBG("recv json data = : %s\r\n",json_buf);

                //解析JSON数据包              
                if(parseJSON(json_buf,&cmd_rx) == NO_ERR)
                {
                    send_to_device(&cmd_rx);
                }
                else
                {
                    //指令解析失败，向上位机发送解析失败的状态，要求重发
                     DBG("parseJSON error\r\n");
                    SendErrcodeToHost(COMM_PARSE_ERR,"cmd parse error\r\n");
                }

                init_serial_boot();   


            }
            else
            {
                DBG("CRC ERROR\r\n");
                dbh("CRC ERROR RxdBuf", (char *)gRecvHost.RxdBuf, gRecvHost.RxdTotalLen);
//                DBG("bccHi = %02x,bccLo = %02x",bccHi,bccLo);
                SendErrcodeToHost(COMM_CRC_ERR,"deal rx data crc error\r\n");

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


uint8_t send_to_host(uint8_t cmd,uint8_t *buf,uint8_t len)
{
    uint8_t i = 0;
    uint8_t json_len = 0;
    uint8_t TxdBuf[MAX_TXD_BUF_LEN]={0};
    uint8_t tmpBuf[MAX_TXD_BUF_LEN] = {0};
    uint16_t iCRC = 0;
    CMD_TX_T cmd_tx;


    memset(tmpBuf,0x00,sizeof(tmpBuf));
    memset(TxdBuf,0x00,sizeof(TxdBuf));
    memset(&cmd_tx,0x00,sizeof(cmd_tx));

    i = 3;
    TxdBuf[0] = STX;
    cmd_tx.cmd = cmd;
    cmd_tx.code = 0;
    
    bcd2asc(cmd_tx.data, buf, len*2, 0);        
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

    dbh("send_to_host",(char *)TxdBuf,i);

#if CMD_SERIAL_PORT == 0x01
    //bsp_Usart1_SendData(TxdBuf,i);
    //add lock avoid data error

    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
    {
        comSendBuf(COM1,TxdBuf,i);        
//        bsp_Usart1_SendData(TxdBuf,i);
    }
    
    xSemaphoreGive(gxMutex);
#else
    bsp_Usart2_SendData(TxdBuf,i);
#endif

    return 0;
}


static SYSERRORCODE_E parseJSON(uint8_t *text,CMD_RX_T *cmd_rx)
{
    cJSON  *root = NULL;			                    // root    
    cJSON  *cmd = NULL;			                        // cmd     
    SYSERRORCODE_E result = NO_ERR;
    
    uint8_t bcd_cmd[2] = {0};
    uint8_t bcd_dat[16] = {0};
    uint8_t *tmpCmd = NULL;
    uint8_t *tmpdat = NULL;
    uint8_t asc_len = 0;  

    //{"cmd":"sensor","value":{"sensor":"A1"}}   
    
    root = cJSON_Parse((const char*)text);

    if (root == NULL)                 // 如果转化错误，则报错退出
    {
        DBG("1.Error before: [%s]\n", cJSON_GetErrorPtr());
        return CJSON_PARSE_ERR;
    }

    //获取KEY,指令描述
    cmd_rx->cmd_desc = (uint8_t *)cJSON_GetObjectItem(root,"cmd")->valuestring;  
    if(cmd_rx->cmd_desc == NULL)
    {
        DBG("2.Error before: [%s]\n", cJSON_GetErrorPtr());
        return CJSON_GETITEM_ERR;
    }


    //获取指令字
    cmd = cJSON_GetObjectItem(root,"value"); 
    if(cmd == NULL)
    {
        DBG("3.Error before: [%s]\n", cJSON_GetErrorPtr());
        return CJSON_GETITEM_ERR;
    }    

    tmpCmd = (uint8_t *)cJSON_GetObjectItem(cmd,(const char*)cmd_rx->cmd_desc)->valuestring; 
    if(tmpCmd == NULL)
    {
        DBG("3.Error before: [%s]\n", cJSON_GetErrorPtr());
        return CJSON_GETITEM_ERR;
    }      


    asc2bcd(bcd_cmd, tmpCmd, strlen((const char*)tmpCmd), 0);

    //目前指令只有1byte 所以直接赋值
    cmd_rx->cmd = bcd_cmd[0];


    memset(tmpdat,0x00,sizeof(tmpdat));
    tmpdat = (uint8_t *)cJSON_GetObjectItem(root,"data")->valuestring;  

    asc_len = strlen((const char*)tmpdat);

    //若是有数据，则转换;无数据则不处理
    if(asc_len > 0)
    {
        if(asc_len % 2 != 0)
        {
            asc_len += 1;
        }
        asc2bcd(bcd_dat,tmpdat,asc_len,0);
        memcpy(cmd_rx->cmd_data,bcd_dat,asc_len/2);
    }
    

    cJSON_Delete(root);

    my_free(cmd);
    my_free(tmpCmd);
    my_free(tmpdat);
    
    return result;

}



static uint8_t  packetJSON(CMD_TX_T *cmd_tx,uint8_t *command_data)
{
    char *TxdBuf;
    cJSON *root; // cJSON指针
    uint8_t len = 0;//返回json的长度

    uint8_t tmp_code = cmd_tx->code;
    uint8_t tmp_data[64] = {0}; 
    
    memset(tmp_data,0x00,sizeof(tmp_data));
    root=cJSON_CreateObject(); // 创建root对象，返回值为cJSON指针

    if (root == NULL)                 // 如果转化错误，则报错退出
    {
        return 0;
    }

    //这里若是不中转则，JSON打包的数据会变，原因未知  2019.07.12 surge
    memcpy(tmp_data,cmd_tx->data,strlen((const char*)cmd_tx->data));
    
    sprintf(TxdBuf,"%02x",cmd_tx->cmd);
    
    cJSON_AddStringToObject(root,"cmd",TxdBuf);
    cJSON_AddNumberToObject(root,"code",tmp_code);
    cJSON_AddStringToObject(root,"data",(const char*)tmp_data);
    
    memset(TxdBuf,0x00,sizeof(TxdBuf));
    TxdBuf = cJSON_PrintUnformatted(root); 

    if(TxdBuf == NULL)
    {
        return 0;
    }

    strcpy((char *)command_data,TxdBuf);

//    DBG("command_data = %s\r\n",TxdBuf);

    len = strlen((const char*)TxdBuf);

    cJSON_Delete(root);

    my_free(TxdBuf);

    return len;
}


void send_to_device(CMD_RX_T *cmd_rx)
{
    uint8_t i = 0;
    uint8_t TxdBuf[MAX_TXD_BUF_LEN]={0};
    uint8_t tmpBuf[MAX_TXD_BUF_LEN] = {0};
    uint16_t iCRC = 0;
    CMD_TX_T cmd_tx;
    
    memset(&cmd_tx,0x00,sizeof(cmd_tx));
    
    switch (cmd_rx->cmd)
    {
        case GETSENSOR://获取红外状态，目前只有两组红外
            i = 3;
            TxdBuf[0] = STX;            
            cmd_tx.cmd = GETSENSOR;
            cmd_tx.code = 0x00;
            bsp_GetSensorStatus(cmd_tx.data);      
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
            i = 3;
            TxdBuf[0] = STX;
            cmd_tx.cmd = SETLED;
            cmd_tx.code = 0x00;
//            DBG("cmd_rx->cmd_data = %s,len = %d\r\n",cmd_rx->cmd_data,strlen((const char*)cmd_rx->cmd_data));
            bsp_Ex_SetLed(cmd_rx->cmd_data); 
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
        case GETDEVICESAT://获取设备状态

            break; 
        case GETVER:
            i = 3;
            TxdBuf[0] = STX;
            cmd_tx.cmd = GETVER;
            cmd_tx.code = 0x00;
            strcpy((char *)cmd_tx.data,"V1.0.1");            
            i += packetJSON(&cmd_tx,tmpBuf); 
            memcpy(TxdBuf+3,tmpBuf,i-3); 
            TxdBuf[i++] = ETX;     

            TxdBuf[1] = i>>8; //high
            TxdBuf[2] = i&0xFF; //low
            
            iCRC = CRC16_Modbus(TxdBuf, i);  
            TxdBuf[i++] = iCRC >> 8;
            TxdBuf[i++] = iCRC & 0xff;   
            break; 
            
        case HEARTBEAT:

            //不需要心跳
            //android一直在发查询指令，可以替换心跳

            break;
        case UPGRADE:
            SystemReset();
            break;   

        case CONTROLMOTOR:       
             //向电机发送控制指令
            comSendBuf(COM4, cmd_rx->cmd_data,8);
            return;//这里不需要向上位机上送，在另外一个任务中才上送
        case DOOR_B:
            //发给B门
            RS485_SendBuf(COM5,cmd_rx->cmd_data,8);
            
            return;//这里不需要向上位机上送，在另外一个任务中才上送

        default:
            init_serial_boot(); 
            return;
    }

#if CMD_SERIAL_PORT == 0x01
    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
    {
        comSendBuf(COM1,TxdBuf,i);
    }
    xSemaphoreGive(gxMutex);

#else
    bsp_Usart2_SendData(TxdBuf,i);
#endif 
}


SYSERRORCODE_E SendErrcodeToHost(SYSERRORCODE_E code,uint8_t *buf)
{
    SYSERRORCODE_E result = NO_ERR;

    uint8_t i = 0;
    uint8_t json_len = 0;
    uint8_t TxdBuf[MAX_TXD_BUF_LEN]={0};
    uint8_t tmpBuf[MAX_TXD_BUF_LEN] = {0};
    uint16_t iCRC = 0;
    CMD_TX_T cmd_tx;

    memset(tmpBuf,0x00,sizeof(tmpBuf));
    memset(TxdBuf,0x00,sizeof(TxdBuf));
    memset(&cmd_tx,0x00,sizeof(cmd_tx));

    i = 3;
    TxdBuf[0] = STX;
    cmd_tx.cmd = 0;
    cmd_tx.code = code;    
    strcpy((char *)tmpBuf,(char *)buf);    

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

    dbh("send_to_host",(char *)TxdBuf,i);


    if(xSemaphoreTake(gxMutex, portMAX_DELAY))
    {
        comSendBuf(COM1,TxdBuf,i);        
    }
    
    xSemaphoreGive(gxMutex);
    
    return result;
}



