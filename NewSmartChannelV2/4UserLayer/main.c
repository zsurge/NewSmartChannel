/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : main.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年7月9日
  最近修改   :
  功能描述   : 主程序模块
  函数列表   :
  修改历史   :
  1.日    期   : 2019年7月9日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "def.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
//#define APPCREATE_TASK_PRIO		(tskIDLE_PRIORITY)
//#define APPCREATE_STK_SIZE 		(configMINIMAL_STACK_SIZE*16)
    


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
//const char *AppCreateTaskName = "vAppCreateTask";      

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
//static TaskHandle_t xHandleTaskAppCreate = NULL;     

SemaphoreHandle_t gxMutex = NULL;
EventGroupHandle_t xCreatedEventGroup = NULL;
QueueHandle_t gxMotorCtrlQueue = NULL; 
QueueHandle_t gxMotorSecDoorCtrlQueue = NULL;
QueueHandle_t gxToHostQueue = NULL;



static void AppTaskCreate(void);
static void AppObjCreate (void);
static void AppPrintf(char *format, ...);


int main(void)
{  
    //硬件初始化
    bsp_Init();

    //创建任务通信机制  
    AppObjCreate();  

    //创建AppTaskCreate任务
    AppTaskCreate();
//    xTaskCreate((TaskFunction_t )AppTaskCreate,     
//                (const char*    )AppCreateTaskName,   
//                (uint16_t       )APPCREATE_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )APPCREATE_TASK_PRIO,
//                (TaskHandle_t*  )&xHandleTaskAppCreate);   

    
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();
    
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    //进入临界区
//    taskENTER_CRITICAL(); 
    
    //握手
//    CreateHandShakeTask();

    //方向指示灯
    CreateLedTask();//5     0

    //与上位机通讯处理
    CreateMsgParseTask();//8 2
    
    //A门电机控制处理
    CreateMotorCtrlTask();//6 1

    //B门电机控制处理
    CreateMotorCtrlSecDoorTask(); //7 6

    //串口任务
//    CreateToHostTask();

    //按键处理
    CreateKeyTask();//4     7

    //读卡器数据收集
    CreateReaderTask();//3  4

    //条码扫描数据处理
    CreateBarCodeTask();//2     5

    //红外传感器数据上送
    CreateSensorTask();//1      3

    //看门狗
    CreateWatchDogTask();//9

    //删除本身
//    vTaskDelete(xHandleTaskAppCreate); //删除AppTaskCreate任务

    //退出临界区
//    taskEXIT_CRITICAL();            

}


/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
//    /* 创建QueuSet */
//    gxQueueSet = xQueueCreateSet(COMBINED_LENGTH);
//	
//	if(gxQueueSet == NULL)
//    {
//        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
//         AppPrintf("创建QueuSet失败\r\n");
//    }


	/* 创建事件标志组 */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
        AppPrintf("创建事件标志组失败\r\n");
    }

	/* 创建互斥信号量 */
    gxMutex = xSemaphoreCreateMutex();
	
	if(gxMutex == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
        AppPrintf("创建互斥信号量失败\r\n");
    }     
    
  
    /* 创建消息队列 */
    gxMotorCtrlQueue = xQueueCreate((UBaseType_t ) MOTORCTRL_QUEUE_LEN,/* 消息队列的长度 */
                              (UBaseType_t ) sizeof(MOTORCTRL_QUEUE_T *));/* 消息的大小 */
    if(gxMotorCtrlQueue == NULL)
    {
        AppPrintf("创建xTransQueue1消息队列失败!\r\n");
    }	

    /* 创建消息队列 */
    gxMotorSecDoorCtrlQueue = xQueueCreate((UBaseType_t ) MOTORCTRL_QUEUE_LEN,/* 消息队列的长度 */
                              (UBaseType_t ) sizeof(MOTORCTRL_QUEUE_T *));/* 消息的大小 */
    if(gxMotorSecDoorCtrlQueue == NULL)
    {
        AppPrintf("创建xTransQueue2消息队列失败!\r\n");
    }	


    /* 创建消息队列 */
    gxToHostQueue = xQueueCreate((UBaseType_t ) MOTORCTRL_QUEUE_LEN,/* 消息队列的长度 */
                              (UBaseType_t ) sizeof(TOHOST_QUEUE_T *));/* 消息的大小 */
    if(gxToHostQueue == NULL)
    {
        AppPrintf("创建gxToHostQueue消息队列失败!\r\n");
    }
    


    /* 添加到queue set时，消息队列和信号量必须为空*/
    /* 添加消息队列和信号量到Queue Set */
//    #define configUSE_QUEUE_SETS 1    
//    xQueueAddToSet(gxMotorCtrlQueue, gxQueueSet);
//    xQueueAddToSet(gxMotorSecDoorCtrlQueue, gxQueueSet);
}

/*
*********************************************************************************************************
*	函 数 名: AppPrintf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
static void  AppPrintf(char *format, ...)
{
    char  buf_str[512 + 1];
    va_list   v_args;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);    

	/* 互斥信号量 */
	xSemaphoreTake(gxMutex, portMAX_DELAY);

    printf("%s", buf_str);

   	xSemaphoreGive(gxMutex);
}








