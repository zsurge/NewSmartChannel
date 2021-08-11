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
#include"Monitor_Task.h"
#include "bsp_led.h"
#include "BSP_Uart.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 6)
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

    uint8_t queryOpen[51] = { 0x02,0x00,0x31,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x38,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x22,0x7d,0x03,0xa5,0xa5 };
    uint8_t opterOpen[53] = { 0x02,0x00,0x33,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x38,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x22,0x7d,0x03,0xa5,0xa5 };

    uint8_t asc[16] = {0};
    
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
        memset(asc,0x00,sizeof(asc));
        
        //获取到，则执行上位机指令，获取不到，则执行状态查询
        xReturn = xQueueReceive( gxMotorCtrlQueue,    /* 消息队列的句柄 */
                                 (void *)&ptMotor,  /*这里获取的是结构体的地址 */
                                 (TickType_t)30); /* 设置阻塞时间 */
        if(pdTRUE == xReturn)
        {
            //消息接收成功，发送接收到的消息
            dbh("recv from host and send to MA:",(char *)ptMotor->data, MOTORCTRL_QUEUE_BUF_LEN);
            RS485_SendBuf(COM4, ptMotor->data,MOTORCTRL_QUEUE_BUF_LEN);//操作A电机
    
//   全绿    
//            r     g
//    L 4C   6C 01 67 00   
//    M 4D   6C 01 67 00
//    R 52   6C 01 67 00

            if(ptMotor->data[5]==0x03)
            {
                //进门，由进口往出口方向开门                
                LED_L_R = 0;
                LED_L_G = 1;
                LED_R_R = 1;
                LED_R_G = 0;
            }
            else if(ptMotor->data[5]==0x02)
            {
                //进门，由出口往出进口方向开门
                LED_L_R = 1;
                LED_L_G = 0;
                LED_R_R = 0;
                LED_R_G = 1;                
            }
        }
        else
        {
            //发送默认数据包
            RS485_SendBuf(COM4, ReadStatus,MOTORCTRL_QUEUE_BUF_LEN);//查询A电机状态
        }

        vTaskDelay(50);
        
  
        if(deal_motor_Parse(COM4,&rxFromHost) != 0)
        { 
            dbh("recv MA and send to host:", rxFromHost.rxBuff,rxFromHost.rxCnt); 
            //send_to_host(CONTROLMOTOR_A,rxFromHost.rxBuff,rxFromHost.rxCnt);       
            if(rxFromHost.rxBuff[3] == 0x08)
            {               
                //关门到位
                LED_L_R = 1;
                LED_L_G = 0;
                LED_R_R = 1;
                LED_R_G = 0;       
                LED_M_R = 1;
                LED_M_G = 0;                  
            }
						 
            if(rxFromHost.rxCnt == 7)
            {
                bcd2asc(asc, rxFromHost.rxBuff, 14, 1);
                
                memcpy(queryOpen+32,asc,14);            

                //if(xSemaphoreTake(gxMutex, portMAX_DELAY))
                //{
                    BSP_UartSend(SCOM1,queryOpen,51); 
                //}
                
                //xSemaphoreGive(gxMutex);      
            }
            else if(rxFromHost.rxCnt == 8)
            {
                bcd2asc(asc, rxFromHost.rxBuff, 16, 1);
                
                memcpy(opterOpen+32,asc,16);            

                //if(xSemaphoreTake(gxMutex, portMAX_DELAY))
                //{
                    BSP_UartSend(SCOM1,opterOpen,53); 
                //}
                
                //xSemaphoreGive(gxMutex);                
            }    			


            
            Motro_A = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));            
        }

        /* 发送事件标志，表示任务正常运行 */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);      
    }

}



