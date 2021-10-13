/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : Key_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年2月25日
  最近修改   :
  功能描述   : 处理按键的任务
  函数列表   :
  修改历史   :
  1.日    期   : 2020年2月25日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "key_task.h"
#include "bsp_key.h"
#include "tool.h"
#include "comm.h"
#include "string.h"


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define KEY_TASK_PRIO	    (tskIDLE_PRIORITY + 4)
#define KEY_STK_SIZE        (configMINIMAL_STACK_SIZE*4)


#define MOTOR_NO1        1
#define MOTOR_NO2        2

#define MAX_TIME_OUT    100


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *keyTaskName = "vKeyTask";     


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskKey = NULL;

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskKey(void *pvParameters);
static void optDoor(uint8_t motorNo);




void CreateKeyTask(void)
{
    //按键
    xTaskCreate((TaskFunction_t )vTaskKey,         
                (const char*    )keyTaskName,       
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )KEY_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskKey); 
}

static void optDoor(uint8_t motorNo)
{
    MOTORCTRL_QUEUE_T *ptMotor; 
    
    uint8_t OpenDoor[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 }; 

    
    ptMotor = &gMotorCtrlQueue;
	/* 初始化结构体指针 */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 	    


    
    if(motorNo == MOTOR_NO1)
    {   
        ptMotor->cmd = CONTROLMOTOR_A;    
        memcpy(ptMotor->data,OpenDoor,MOTORCTRL_QUEUE_BUF_LEN); 
        
        /* 使用消息队列实现指针变量的传递 */
        if(xQueueSend(gxMotorCtrlQueue,             /* 消息队列句柄 */
                     (void *) &ptMotor,             /* 发送结构体指针变量ptReader的地址 */
                     (TickType_t)30) != pdPASS )
        {
            xQueueReset(gxMotorCtrlQueue);            
        } 
    }
    else
    {
        ptMotor->cmd = CONTROLMOTOR_B; 
        memcpy(ptMotor->data,OpenDoor,MOTORCTRL_QUEUE_BUF_LEN); 
        
        /* 使用消息队列实现指针变量的传递 */
        if(xQueueSend(gxMotorSecDoorCtrlQueue,      /* 消息队列句柄 */
                     (void *) &ptMotor,             /* 发送结构体指针变量ptReader的地址 */
                     (TickType_t)30) != pdPASS )
        {
            xQueueReset(gxMotorSecDoorCtrlQueue);
        }


    }
}


static void vTaskKey(void *pvParameters)
{ 
    int32_t iTime1, iTime2;

    
    while(1)
    {
        switch (Key_Scan(GPIO_PORT_KEY, GPIO_PIN_KEY_DOOR_B))
        {
            case KEY_ON:    
                iTime1 = xTaskGetTickCount();   /* 记下开始时间 */              
                KeyOpenDoorB();
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:  
                break;
            case KEY_ERROR:              
                break;
            default:
                break;
        }

        switch (Key_Scan(GPIO_PORT_KEY, GPIO_PIN_FIREFIGHTING))
        {
            case KEY_ON:       
                iTime1 = xTaskGetTickCount();   /* 记下开始时间 */
                optDoor(MOTOR_NO1);
                optDoor(MOTOR_NO2);
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:  
                break;
            case KEY_ERROR:              
                break;
            default:
                break;

        }

        switch (Key_Scan(GPIO_PORT_OPEN_DOOR, GPIO_PIN_OPEN_DOOR_A))
        {
            case KEY_ON:     
                iTime1 = xTaskGetTickCount();   /* 记下开始时间 */
                optDoor(MOTOR_NO1);
                
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:   
                break;
            case KEY_ERROR:              
                break;
            default:
                break;

        }        

        switch (Key_Scan(GPIO_PORT_OPEN_DOOR, GPIO_PIN_OPEN_DOOR_B))
        {
            case KEY_ON:   
                iTime1 = xTaskGetTickCount();   /* 记下开始时间 */
                optDoor(MOTOR_NO2);
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:
                break;
            case KEY_ERROR:              
                break;
            default:
                break;
        }


        /* 发送事件标志，表示任务正常运行 */
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_7);
		
		vTaskDelay(20);
	}   

}






