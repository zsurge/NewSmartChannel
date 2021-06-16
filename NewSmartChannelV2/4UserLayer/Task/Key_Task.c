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
#define KEY_STK_SIZE        (configMINIMAL_STACK_SIZE*6)

#define FIRSTDOOR_ISOPEN_YES    1
#define FIRSTDOOR_ISOPEN_NO     0

#define SECDOOR_ISOPEN_YES    1
#define SECDOOR_ISOPEN_NO     0

#define MOTOR_NO1        1
#define MOTOR_NO2        2


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *keyTaskName = "vKeyTask";     

static uint8_t isFirstOpen = FIRSTDOOR_ISOPEN_YES;
static uint8_t isSecondOpen = SECDOOR_ISOPEN_YES;

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

static void vTaskKey(void *pvParameters)
{ 
	uint8_t ucKeyCode;
	
//	uint8_t pcWriteBuffer[1024];
//    uint32_t g_memsize;  
//    int32_t iTime1, iTime2;

    
    while(1)
    {
        ucKeyCode = bsp_key_Scan(0);      
		
		if (ucKeyCode != KEY_NONE)
		{
            DBG("ucKeyCode = %d\r\n",ucKeyCode);
              
			switch (ucKeyCode)
			{
				/* K1键按下 打印任务执行情况 */
				case KEY_DOOR_B_PRES:	   
					DBG("KEY_DOOR_B_PRES is press\r\n");					
//	                iTime1 = xTaskGetTickCount();   /* 记下开始时间 */
//	                iTime2 = xTaskGetTickCount();   /* 记下结束时间 */
//	                DBG ( "修改记录成功，耗时: %dms\r\n",iTime2 - iTime1 );
	                
//					printf("=================================================\r\n");
//					printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
//					vTaskList((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);
//                    
//					printf("\r\n任务名       运行计数         使用率\r\n");
//					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);          

//                    g_memsize = xPortGetFreeHeapSize();
//                    printf("系统当前内存大小为 %d 字节，开始申请内存\n",g_memsize);
                    
//                    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");
                    KeyOpenDoorB();
					break;				
				/* K2键按下，打印串口操作命令 */
				case KEY_FIREFIGHTING_PRES:
				    DBG("KEY_FIREFIGHTING_PRES is press\r\n");
                    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");
                    optDoor(MOTOR_NO1);
				    optDoor(MOTOR_NO2);
					break;
				case KEY_OPEN_DOOR_A_PRES: 
				    DBG("KEY_OPEN_DOOR_A_PRES is press\r\n");				    
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_A,NO_ERR,"Open door A manually"); 
                    optDoor(MOTOR_NO1);
					break;
				case KEY_OPEN_DOOR_B_PRES: 
				    DBG("KEY_OPEN_DOOR_B_PRES is press\r\n");                    
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_B,NO_ERR,"Open door B manually");
                    optDoor(MOTOR_NO2);
					break;                
				
				/* 其他的键值不处理 */
				default:   
				    DBG("KEY_default\r\n");
					break;
			}
		}
		

        /* 发送事件标志，表示任务正常运行 */
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_7);
		
		vTaskDelay(30);
	}  

}


static void optDoor(uint8_t motorNo)
{
    MOTORCTRL_QUEUE_T *ptMotor; 
    
    uint8_t OpenDoor[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 }; 
    uint8_t CloseDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x01,0x8A,0x69 };
    
    ptMotor = &gMotorCtrlQueue;
	/* 初始化结构体指针 */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 	    


//    DBG("motorNo = %d,isFirstOpen = %d,isSecondOpen = %d\r\n",motorNo,isFirstOpen,isSecondOpen);
    
    if(motorNo == MOTOR_NO1)
    {   
        ptMotor->cmd = CONTROLMOTOR_A;
        
//        if(isFirstOpen)
//        {
            memcpy(ptMotor->data,OpenDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }
//        else
//        {
//            memcpy(ptMotor->data,CloseDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }

//        isFirstOpen = !isFirstOpen;
                
    
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
        
//        if(isSecondOpen)
//        {
            memcpy(ptMotor->data,OpenDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }
//        else
//        {
//            memcpy(ptMotor->data,CloseDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }

//        
//        isSecondOpen = !isSecondOpen;
        
        /* 使用消息队列实现指针变量的传递 */
        if(xQueueSend(gxMotorSecDoorCtrlQueue,      /* 消息队列句柄 */
                     (void *) &ptMotor,             /* 发送结构体指针变量ptReader的地址 */
                     (TickType_t)30) != pdPASS )
        {
            xQueueReset(gxMotorSecDoorCtrlQueue);
        }


    }
}



