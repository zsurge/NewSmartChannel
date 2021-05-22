/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : MotorCtrl_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年2月26日
  最近修改   :
  功能描述   : 电机控制任务
  函数列表   :
  修改历史   :
  1.日    期   : 2020年2月26日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include"MotorCtrl_Task.h"
#include "bsp_uart_fifo.h"
#include "comm.h"
#include "string.h"



/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 7)
#define MOTOR_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)



/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *MotorCtrlTaskName = "vMotorCtrlTask";      

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMotorCtrl = NULL;     




/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskMotorCtrl(void *pvParameters);



void CreateMotorCtrlTask(void)
{
    //创建电机信息返回任务
    xTaskCreate((TaskFunction_t )vTaskMotorCtrl,     
                (const char*    )MotorCtrlTaskName,   
                (uint16_t       )MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotorCtrl);     

}

static void vTaskMotorCtrl(void *pvParameters)
{  
    BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */
//    uint32_t i = 0;
//    uint8_t buf[8] = {0};
//    uint16_t readLen = 0;
//    uint16_t iCRC = 0;
//    uint8_t crcBuf[2] = {0};
//    uint8_t CloseDoor[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x06,0x08,0x0C,0x00,0x01,0x8A,0x69 };
    uint8_t ReadStatus[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
//    uint8_t resetMotor[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x06,0x08,0x0C,0x00,0x07,0x0A,0x6B };
//    uint32_t NotifyValue = 0x55;
    FROMHOST_STRU rxFromHost;   
    MOTORCTRL_QUEUE_T *ptMotor; 
	/* 初始化结构体指针 */
	ptMotor = &gMotorCtrlQueue;
    
    /* 清零 */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    { 
        //获取到，则执行上位机指令，获取不到，则执行状态查询
        xReturn = xQueueReceive( gxMotorCtrlQueue,    /* 消息队列的句柄 */
                                 (void *)&ptMotor,  /*这里获取的是结构体的地址 */
                                 (TickType_t)30); /* 设置阻塞时间 */
        if(pdTRUE == xReturn)
        {
            //消息接收成功，发送接收到的消息
            dbh("A queue recv:",(char *)ptMotor->data, MOTORCTRL_QUEUE_BUF_LEN);
            RS485_SendBuf(COM4, ptMotor->data,MOTORCTRL_QUEUE_BUF_LEN);//操作A电机  

            //判定是否是关门指令
//            if(memcmp(ptMotor->data,CloseDoor,MOTORCTRL_QUEUE_BUF_LEN) == 0)
//            {                
//                printf("the door is closing,enable monitor task\r\n");
//                NotifyValue = 0x55;
//                vTaskResume(xHandleTaskMonitor);            
//            }
        
        }
        else
        {
            //发送默认数据包
            RS485_SendBuf(COM4, ReadStatus,MOTORCTRL_QUEUE_BUF_LEN);//查询A电机状态
        }  
        

        vTaskDelay(50);
        
         #if 1 
        if(deal_motor_Parse(COM4,&rxFromHost) != 0)
        { 
//            dbh("a send host", rxFromHost.rxBuff,rxFromHost.rxCnt);   
//            i = 0;
            send_to_host(CONTROLMOTOR_A,rxFromHost.rxBuff,rxFromHost.rxCnt);              
            Motro_A = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));            
        }
        #else
        
      
        readLen = RS485_Recv(COM4,buf,8);  
        
        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            {  

                dbh("a send host",buf,readLen);
                i = 0;
                send_to_host(CONTROLMOTOR_A,buf,readLen);              
                Motro_A = 0;

                //这里判定是否是关到位,0x08代表电机关到位
//                if(buf[3] == 0x08 && NotifyValue == 0x55)
//                {
//                    //置信号量，停用监控任务
//                    printf("the door is closed,disable monitor task\r\n");
//                    NotifyValue = 0xAA;
//                    vTaskSuspend(xHandleTaskMonitor); 
//                }                
            }
        } 
//        else
//        {
//            if(i++ == READ_MOTOR_STATUS_TIMES)
//            {
//                i = 0;
//                DBG("door a connect error!\r\n"); 
//                //RS485_SendBuf(COM4, resetMotor,MOTORCTRL_QUEUE_BUF_LEN);//查询A电机状态
//                
//                SendAsciiCodeToHost(ERRORINFO,MOTOR_A_ERR,"Motor A fault");
//            }
//            
//        }

#endif

        /* 发送事件标志，表示任务正常运行 */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);
      
    }

}



