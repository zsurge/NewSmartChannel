/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : MotorCtrl_SecDoor_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年4月28日
  最近修改   :
  功能描述   : 第二个电机控制部分
  函数列表   :
  修改历史   :
  1.日    期   : 2020年4月28日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/


/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include"MotorCtrl_SecDoor_Task.h"
#include "bsp_uart_fifo.h"
#include "comm.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define MOTOR_SEC_TASK_PRIO		( tskIDLE_PRIORITY + 4)
#define MOTOR_SEC_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *MotorCtrlSecDoorTaskName = "vSecMotorTask";      

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMotorCtrlSecDoor = NULL;  

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskMotorCtrlSecDoor(void *pvParameters);



void CreateMotorCtrlSecDoorTask(void)
{
    //创建电机信息返回任务
    xTaskCreate((TaskFunction_t )vTaskMotorCtrlSecDoor,     
                (const char*    )MotorCtrlSecDoorTaskName,   
                (uint16_t       )MOTOR_SEC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_SEC_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotorCtrlSecDoor);     

}

static void vTaskMotorCtrlSecDoor(void *pvParameters)
{  
    BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */
//    uint32_t i = 0;
    
//    uint8_t resetMotor[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x06,0x08,0x0C,0x00,0x07,0x0A,0x6B };
    uint8_t ReadStatus[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
    FROMHOST_STRU rxFromHost;   

    MOTORCTRL_QUEUE_T *ptMotor; 
	/* 初始化结构体指针 */
	ptMotor = &gSecMotorCtrlQueue;

    /* 清零 */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    {   

        //获取到，则执行上位机指令，获取不到，则执行状态查询
        xReturn = xQueueReceive( gxMotorSecDoorCtrlQueue,    /* 消息队列的句柄 */
                                 (void *)&ptMotor,          /*这里获取的是结构体的地址 */
                                 (TickType_t)50);          /* 设置阻塞时间 */
        if(pdTRUE == xReturn)
        {
            //消息接收成功，发送接收到的消息
//            dbh("B queue recv",(char *)ptMotor->data, MOTORCTRL_QUEUE_BUF_LEN);
            RS485_SendBuf(COM5, ptMotor->data,MOTORCTRL_QUEUE_BUF_LEN);//操作B电机  

        }
        else
        {
//            DBG("Send B door query status\r\n");
            //发送默认数据包
            RS485_SendBuf(COM5, ReadStatus,MOTORCTRL_QUEUE_BUF_LEN);//查询B电机状态
            
        }                            

        vTaskDelay(80);

        if(deal_motor_Parse(COM5,&rxFromHost) != 0)
        {
//            dbh("B send host", rxFromHost.rxBuff,rxFromHost.rxCnt);   
//            i = 0;
            send_to_host(CONTROLMOTOR_B,rxFromHost.rxBuff,rxFromHost.rxCnt);              
            Motro_B = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));
        }

#if 0        
        readLen = RS485_Recv(COM5,buf,8);     
//        readLen = bsp_Usart5_Read(buf,8);
        dbh("b door", buf, readLen);
        
        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            {    
                i = 0;
                send_to_host(CONTROLMOTOR_B,buf,readLen);              
                Motro_B = 0;
            }
        } 
//        else
//        {
//            if(i++ == READ_MOTOR_STATUS_TIMES)
//            {
//                i = 0;
//                DBG("door b connect error!\r\n");                 
//                //RS485_SendBuf(COM5, resetMotor,MOTORCTRL_QUEUE_BUF_LEN);//RESET电机状态
//                SendAsciiCodeToHost(ERRORINFO,MOTOR_B_ERR,"Motor B fault");
//            }
//            
//        }

#endif

        /* 发送事件标志，表示任务正常运行 */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
      
    }

}






