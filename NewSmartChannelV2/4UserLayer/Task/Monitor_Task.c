/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : Monitor_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年5月25日
  最近修改   :
  功能描述   : 监控线程，做为防夹使用
  函数列表   :
  修改历史   :
  1.日    期   : 2020年5月25日
    作    者   :  
    修改内容   : 创建文件

    监控到关门时，启用该任务，读取红外状态，执行防夹功能
    收到关门到位或者开门到位，挂起该任务

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include"Monitor_Task.h"
#include "comm.h"
#include "bsp_sensor.h"
#include "stdlib.h"
#include "bsp_uart_fifo.h"

    
    
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define MONITOR_TASK_PRIO		(tskIDLE_PRIORITY + 4)
#define MONITOR_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)

#define MON_ENABLE  0x55
#define MON_DISABLE  0xAA



/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *MonitorTaskName = "vMonitorTask";    

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMonitor = NULL; 

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskMonitor(void *pvParameters);



void CreateMonitorTask(void)
{
    //创建电机信息返回任务
    xTaskCreate((TaskFunction_t )vTaskMonitor,     
                (const char*    )MonitorTaskName,   
                (uint16_t       )MONITOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MONITOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMonitor);
}


#if 0
static void vTaskMonitor(void *pvParameters)
{
    BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */
    uint32_t r_num = 0;
    uint8_t buf[8] = {0};
    uint8_t OpenDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 };
    static uint32_t flag = MON_DISABLE;

    while (1)
    {
        //获取任务通知 ,没获取到则一直等待
    	xReturn=xTaskNotifyWait(0x0,	  //进入函数的时候不清除任务bit
                            ULONG_MAX,	  //退出函数的时候清除所有的bit
                            &r_num,		  //保存任务通知值                     
                            portMAX_DELAY);	//阻塞时间
                            
        if( pdTRUE == xReturn || flag == MON_ENABLE)
        {           
            //第一次会赋值，一直到第二次接收到通知为止
            flag = r_num;
            
            bsp_GetSensorValue(buf);
//            printf("r_num == %x,sensor = %s,flag = %x\r\n",r_num,buf,flag);
            if(atoi((const char*)buf) != 0 && flag != MON_DISABLE) 
            {
                printf("waring!!! open door\r\n");
                RS485_SendBuf(COM4, OpenDoor,sizeof(OpenDoor));//打开A电机
            }
        }
        
         vTaskDelay(20);
        
    }    
    
}
#endif 


#if 0
static void vTaskMonitor(void *pvParameters)
{
	QueueSetMemberHandle_t xActivatedMember;
    MOTORCTRL_QUEUE_T *ptMotor1; 
    MOTORCTRL_QUEUE_T *ptMotor2; 
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(50); 

    while (1)
    {
		/* 多事件等待，等待二值信号量和消息队列 */
        xActivatedMember = xQueueSelectFromSet(gxQueueSet, xMaxBlockTime);

        /* 根据xActivatedMember判断接受到的消息类型 */
        if(xActivatedMember == gxMotorCtrlQueue)
        {
			/* 消息队列1接收到消息 */
            xQueueReceive(xActivatedMember, &ptMotor1, 0);
			dbh("ptMotor1", ptMotor1->data,sizeof(ptMotor1->data));
        }
        else if(xActivatedMember == gxMotorSecDoorCtrlQueue)
        {
			/* 消息队列2接收到消息 */
            xQueueReceive(xActivatedMember, &ptMotor2, 0);
			dbh("ptMotor2", ptMotor2->data,sizeof(ptMotor2->data));
        }
        else
        {
            ;
        }        
    }    
    
}

#endif





static void vTaskMonitor(void *pvParameters)
{
    uint8_t buf[8] = {0};
    uint8_t OpenDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 };
    static uint32_t flag = MON_DISABLE;

    while (1)
    {
        if(flag == MON_DISABLE )
        {
            flag = MON_ENABLE ;
            vTaskSuspend(xHandleTaskMonitor);
        }

        bsp_GetSensorValue(buf);

        if(atoi((const char*)buf) != 0) 
        {
            printf("waring!!! open door\r\n");
            RS485_SendBuf(COM4, OpenDoor,sizeof(OpenDoor));//打开A电机
            vTaskSuspend(xHandleTaskMonitor);
        }
        
        vTaskDelay(10);
        
    }    
    
}












