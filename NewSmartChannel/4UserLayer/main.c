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
//任务优先级   

static void check_msg_queue(void);

#define LED_TASK_PRIO	    ( tskIDLE_PRIORITY)
#define HANDSHAKE_TASK_PRIO	( tskIDLE_PRIORITY)
#define QUERYMOTOR_TASK_PRIO ( tskIDLE_PRIORITY)
#define READER_TASK_PRIO	( tskIDLE_PRIORITY + 1)
#define QR_TASK_PRIO	    ( tskIDLE_PRIORITY + 1)
#define KEY_TASK_PRIO	    ( tskIDLE_PRIORITY + 2)
#define INFRARED_TASK_PRIO	( tskIDLE_PRIORITY + 2)
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define RS485_TASK_PRIO	    ( tskIDLE_PRIORITY + 3)
#define CMD_TASK_PRIO		( tskIDLE_PRIORITY + 4)
#define START_TASK_PRIO		( tskIDLE_PRIORITY + 5)



//任务堆栈大小   
//#define LED_STK_SIZE 		(1024)
//#define MOTOR_STK_SIZE 		(1024) 
//#define CMD_STK_SIZE 		(1024*2)
//#define INFRARED_STK_SIZE 	(1024)
//#define RS485_STK_SIZE 		(1024)
//#define START_STK_SIZE 	    (256)
//#define QR_STK_SIZE 		(1024)
//#define READER_STK_SIZE     (1024)
//#define HANDSHAKE_STK_SIZE  (1024)
//#define KEY_STK_SIZE        (512)
//#define QUERYMOTOR_STK_SIZE      (512)


#define LED_STK_SIZE 		(1024)
#define MOTOR_STK_SIZE 		(1024) 
#define CMD_STK_SIZE 		(1024*1)
#define INFRARED_STK_SIZE 	(1024)
#define RS485_STK_SIZE 		(1024)
#define START_STK_SIZE 	    (512)
#define QR_STK_SIZE 		(1280)
#define READER_STK_SIZE     (1024)
#define HANDSHAKE_STK_SIZE  (1024)
#define KEY_STK_SIZE        (1024)
#define QUERYMOTOR_STK_SIZE      (512)

//事件标志
#define TASK_BIT_0	 (1 << 0)
#define TASK_BIT_1	 (1 << 1)
#define TASK_BIT_2	 (1 << 2)
#define TASK_BIT_3	 (1 << 3)
#define TASK_BIT_4	 (1 << 4)
#define TASK_BIT_5	 (1 << 5)
#define TASK_BIT_6	 (1 << 6)
#define TASK_BIT_7	 (1 << 7)
#define TASK_BIT_8	 (1 << 8)

//读取电机状态最大次数
#define READ_MOTOR_STATUS_TIMES 20


//#define TASK_BIT_ALL (TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 | TASK_BIT_3|TASK_BIT_4 | TASK_BIT_5 | TASK_BIT_6 )
#define TASK_BIT_ALL ( TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 |TASK_BIT_3|TASK_BIT_4|TASK_BIT_5|TASK_BIT_6|TASK_BIT_7|TASK_BIT_8)

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
//任务句柄
static TaskHandle_t xHandleTaskLed = NULL;      //LED灯
static TaskHandle_t xHandleTaskMotor = NULL;    //电机控制
static TaskHandle_t xHandleTaskCmd = NULL;      //android通讯
static TaskHandle_t xHandleTaskInfrared = NULL; //红外感映
static TaskHandle_t xHandleTaskReader = NULL;   //韦根读卡器
static TaskHandle_t xHandleTaskQr = NULL;       //二维码读头
static TaskHandle_t xHandleTaskRs485 = NULL;    //B门电机
static TaskHandle_t xHandleTaskStart = NULL;    //看门狗
static TaskHandle_t xHandleTaskHandShake = NULL;    // 握手
static TaskHandle_t xHandleTaskKey = NULL;      //B门按键

//事件句柄
static EventGroupHandle_t xCreatedEventGroup = NULL;


//可以做为脱机模式情况下，红外，读卡器，二维码来开门
//脱机模式，判定读卡器的编码范围，以及二维码的计算规则
#ifdef USEQUEUE
#define MONITOR_TASK_PRIO	( tskIDLE_PRIORITY + 4)
#define MONITOR_STK_SIZE   (1024)
static TaskHandle_t xHandleTaskMonitor = NULL;    //监控任务
static void vTaskMonitor(void *pvParameters);
static QueueHandle_t xTransQueue = NULL;
#endif


volatile int8_t mau_key = 0;

static QueueHandle_t xTransKeyQueue = NULL;
static QueueHandle_t xTransMotorBQueue = NULL;


#define  KEY_QUEUE_LEN    10   /* 队列的长度，最大可包含多少个消息 */
#define  KEY_QUEUE_SIZE   4   /* 队列中每个消息大小（字节） */


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

//任务函数
static void vTaskLed(void *pvParameters);
static void vTaskMortorToHost(void *pvParameters);
static void vTaskKey(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskInfrared(void *pvParameters);
static void vTaskRs485(void *pvParameters);
static void vTaskReader(void *pvParameters);
static void vTaskQR(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void vTaskQueryMotor(void *pvParameters);
//上送开机次数
static void vTaskHandShake(void *pvParameters);


static void AppTaskCreate(void);
static void AppObjCreate (void);
static void App_Printf(char *format, ...);


//static void AppEventCreate (void);

//static void DisplayDevInfo (void);

//static void DisplayDevInfo(void)
//{
//	printf("Softversion :%s\r\n",gDevinfo.SoftwareVersion);
//  printf("HardwareVersion :%s\r\n", gDevinfo.HardwareVersion);
//	printf("Model :%s\r\n", gDevinfo.Model);
//	printf("ProductBatch :%s\r\n", gDevinfo.ProductBatch);	    
//	printf("BulidDate :%s\r\n", gDevinfo.BulidDate);
//	printf("DevSn :%s\r\n", gDevinfo.GetSn());
//}


int main(void)
{   
    //硬件初始化
    bsp_Init();  

	/* 创建任务通信机制 */
	AppObjCreate();

	/* 创建任务 */
	AppTaskCreate();
    
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
    //跟android握手
    xTaskCreate((TaskFunction_t )vTaskHandShake,
                (const char*    )"vHandShake",       
                (uint16_t       )HANDSHAKE_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )HANDSHAKE_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskHandShake);  

    //创建LED任务
    xTaskCreate((TaskFunction_t )vTaskLed,         
                (const char*    )"vTaskLed",       
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )LED_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskLed);                   

    //查询电机状态
    xTaskCreate((TaskFunction_t )vTaskQueryMotor,
                (const char*    )"vQueryMotor",       
                (uint16_t       )QUERYMOTOR_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )QUERYMOTOR_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskQueryMotor);  
    
    //创建电机信息返回任务
    xTaskCreate((TaskFunction_t )vTaskMortorToHost,     
                (const char*    )"vTMTHost",   
                (uint16_t       )MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotor);     

    //跟android通讯串口数据解析
    xTaskCreate((TaskFunction_t )vTaskMsgPro,     
                (const char*    )"cmd",   
                (uint16_t       )CMD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CMD_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskCmd);      

    //红外传感器状态上送
    xTaskCreate((TaskFunction_t )vTaskInfrared,     
                (const char*    )"vInfrared",   
                (uint16_t       )INFRARED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )INFRARED_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskInfrared);    


    //全高门电机状态返回
    xTaskCreate((TaskFunction_t )vTaskRs485,     
                (const char*    )"vRs485",   
                (uint16_t       )RS485_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RS485_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskRs485);  

    //韦根读卡器
    xTaskCreate((TaskFunction_t )vTaskReader,     
                (const char*    )"vReader",   
                (uint16_t       )READER_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )READER_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskReader);    

    //二维码扫码模块
    xTaskCreate((TaskFunction_t )vTaskQR,     
                (const char*    )"vTaskQR",   
                (uint16_t       )QR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )QR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskQr);      

    //B门按键
    xTaskCreate((TaskFunction_t )vTaskKey,         
                (const char*    )"vTaskKey",       
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )KEY_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskKey);   

    #ifdef USEQUEUE
    //监控线程
    xTaskCreate((TaskFunction_t )vTaskMonitor,     
                (const char*    )"vTaskMonitor",   
                (uint16_t       )MONITOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MONITOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMonitor);
    #endif

    //看门狗
	xTaskCreate((TaskFunction_t )vTaskStart,     		/* 任务函数  */
                (const char*    )"vTaskStart",   		/* 任务名    */
                (uint16_t       )START_STK_SIZE,        /* 任务栈大小，单位word，也就是4字节 */
                (void*          )NULL,           		/* 任务参数  */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级*/
                (TaskHandle_t*  )&xHandleTaskStart );   /* 任务句柄  */                

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
	/* 创建事件标志组 */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
        App_Printf("创建事件标志组失败\r\n");
    }

	/* 创建互斥信号量 */
    gxMutex = xSemaphoreCreateMutex();
	
	if(gxMutex == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
        App_Printf("创建互斥信号量失败\r\n");
    }    

    //创建二值信号量
//    gBinarySem_Handle = xSemaphoreCreateBinary();

//    if(gBinarySem_Handle == NULL)
//    {
//        App_Printf("创建二值信号量失败\r\n");
//    }

    #ifdef USEQUEUE
    /* 创建消息队列 */
    xTransQueue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* 消息队列的长度 */
                              (UBaseType_t ) sizeof(QUEUE_TO_HOST_T *));/* 消息的大小 */
    if(xTransQueue == NULL)
    {
        App_Printf("创建xTransQueue消息队列失败!\r\n");
    }	
    #endif


    /* 创建消息队列 */
    xTransKeyQueue = xQueueCreate((UBaseType_t ) KEY_QUEUE_LEN,/* 消息队列的长度 */
                            (UBaseType_t )KEY_QUEUE_SIZE);/* 消息的大小 */
    if(xTransKeyQueue == NULL)
    {
        App_Printf("xTransKeyQueue error!\r\n");
    }	

    xTransMotorBQueue = xQueueCreate((UBaseType_t ) KEY_QUEUE_LEN,/* 消息队列的长度 */
                            (UBaseType_t )KEY_QUEUE_SIZE);/* 消息的大小 */
    if(xTransMotorBQueue == NULL)
    {
        App_Printf("xTransKeyQueue error!\r\n");
    }	


}



/*
*********************************************************************************************************
*	函 数 名: vTaskStart
*	功能说明: 启动任务，等待所有任务发事件标志过来。
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
	EventBits_t uxBits;
	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* 最大延迟1000ms */   
    
	/* 
	  开始执行启动任务主函数前使能独立看门狗。
	  设置LSI是32分频，下面函数参数范围0-0xFFF，分别代表最小值1ms和最大值4095ms
	  下面设置的是4s，如果4s内没有喂狗，系统复位。
	*/
	bsp_InitIwdg(4000);
	
	/* 打印系统开机状态，方便查看系统是否复位 */
	App_Printf("=====================================================\r\n");
	App_Printf("系统开机执行\r\n");
	App_Printf("=====================================================\r\n");
	
    while(1)
    {   
        
		/* 等待所有任务发来事件标志 */
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
							         TASK_BIT_ALL,       /* 等待TASK_BIT_ALL被设置 */
							         pdTRUE,             /* 退出前TASK_BIT_ALL被清除，这里是TASK_BIT_ALL都被设置才表示“退出”*/
							         pdTRUE,             /* 设置为pdTRUE表示等待TASK_BIT_ALL都被设置*/
							         xTicksToWait); 	 /* 等待延迟时间 */
		
		if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
		{  
		    IWDG_Feed(); //喂狗			
		}
	    else
		{
			/* 基本是每xTicksToWait进来一次 */
			/* 通过变量uxBits简单的可以在此处检测那个任务长期没有发来运行标志 */

            //时序原因，值不太准确，需要更精准的方法

//            if((uxBits & TASK_BIT_0) != 0x01)
//            {
//                DBG("BIT_0 vTaskLed error = %02x,%02x   %02x \r\n",(uxBits & TASK_BIT_0),uxBits,TASK_BIT_0);
//            }

//            if((uxBits & TASK_BIT_1) != 0x02)
//            {
//                DBG("BIT_1 vTaskMotorToHost error = %02x, %02x   %02x \r\n",(uxBits & TASK_BIT_1),uxBits,TASK_BIT_1);
//            }

//            if((uxBits & TASK_BIT_2) != 0x04)
//            {
//                DBG("BIT_2 vTaskMsgPro error = %02x, %02x   %02x \r\n",(uxBits & TASK_BIT_2),uxBits,TASK_BIT_2);
//            }
//            
//            if((uxBits & TASK_BIT_3) != 0x08)
//            {
//                DBG("BIT_3 vTaskInfrared error = %02x, %02x   %02x \r\n",(uxBits & TASK_BIT_3),uxBits,TASK_BIT_3);
//            }

//            if((uxBits & TASK_BIT_4) != 0x10)
//            {
//                DBG("BIT_4 vTaskReader error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_4),uxBits,TASK_BIT_4);
//            }

//            if((uxBits & TASK_BIT_5) != 0x20)
//            {
//                DBG("BIT_5 vTaskQR error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_5),uxBits,TASK_BIT_5);
//            }       

//            if((uxBits & TASK_BIT_6) != 0x40)
//            {
//                DBG("BIT_6 vTaskRs485 error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_6),uxBits,TASK_BIT_6);
//            }  

//            if((uxBits & TASK_BIT_7) != 0x80)
//            {
//                DBG("BIT_7 vTaskKey error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_7),uxBits,TASK_BIT_7);
//            } 

//            if((uxBits & (TASK_BIT_8>>8)) != 0x01)
//            {
//                DBG("BIT_8 vTaskQueryMotor error = %04x,%02x   ,%02x \r\n",(uxBits & (TASK_BIT_8>>8)),uxBits,TASK_BIT_8);
//            } 

            
		}
    }
}







//查询电机状态
void vTaskQueryMotor(void *pvParameters)
{
    uint8_t ReadStatus[8] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
//    BaseType_t xReturn = pdPASS;
    
    while(1)
    {
//        xReturn = xSemaphoreTake(gBinarySem_Handle,portMAX_DELAY); 

//        if(xReturn == pdFALSE)
//        {
//            comSendBuf(COM4, ReadStatus,8);//查询A电机状态
            RS485_SendBuf(COM4,ReadStatus,8);
            RS485_SendBuf(COM5,ReadStatus,8);//查询B电机状态
           
//        }
     
		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_8);  
        vTaskDelay(500);     
    }

} 

//LED任务函数 
void vTaskLed(void *pvParameters)
{   
    //uint8_t pcWriteBuffer[512];
    
    uint8_t i = 0;
    BEEP = 0;
    vTaskDelay(300);
    BEEP = 1;
    
    while(1)
    {  
//        if(g500usCount == 0)
//        {
//            g500usCount = 1*60*1000;//30ms

//            App_Printf("\r\n=================================================\r\n");
//            App_Printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
//            vTaskList((char *)&pcWriteBuffer);
//            App_Printf("%s\r\n", pcWriteBuffer);

//            App_Printf("\r\n任务名       运行计数         使用率\r\n");
//            vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//            App_Printf("%s\r\n", pcWriteBuffer);      
//        }
        
        if(Motro_A== 1)
        {
          LED3=!LED3;   
        }
        else
        {
            LED3 = 0;
        }
        
        if(Motro_B == 1)
        {
          LED2=!LED2;   
        }
        else
        {
            LED2 = 0;
        }    

        //系统状态运行灯，每500ms 一次
        i++;
        if(i == 5)
        {
            i = 0;
            LED4=!LED4; 
        }
        
		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_0);  
        vTaskDelay(100);     
    }
}   

//motor to host 任务函数
void vTaskMortorToHost(void *pvParameters)
{     
    uint8_t buf[8] = {0};
    uint16_t readLen = 0;
    uint16_t iCRC = 0;
    uint8_t crcBuf[2] = {0};
    
    BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
    uint32_t motora = 55;

    

    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptMotorToHost;  
    ptMotorToHost = &gQueueToHost;
    #endif
    
    while (1)
    {   
        readLen = RS485_Recv(COM4,buf,8); 

        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            { 
                #ifdef USEQUEUE
                ptMotorToHost->cmd = CONTROLMOTOR;
                memcpy(ptMotorToHost->data,buf,readLen);

    			/* 使用消息队列实现指针变量的传递 */
    			if(xQueueSend(xTransQueue,              /* 消息队列句柄 */
    						 (void *) &ptMotorToHost,   /* 发送结构体指针变量ptQueueToHost的地址 */
    						 (TickType_t)10) != pdPASS )
    			{
                    DBG("向xTransQueue发送数据失败，即使等待了10个时钟节拍\r\n");                
                } 
                else
                {
//                    DBG("向xTransQueue发送数据成功\r\n");
                      dbh("CONTROLMOTOR",(char *)buf,readLen);
                }    
                #endif

                if(readLen == 8 && mau_key >= 55)
                {
                    xReturn = xQueueSend( xTransKeyQueue, /* 消息队列的句柄 */
                                        &motora,/* 发送的消息内容 */
                                        (TickType_t)30 );        /* 等待时间 0 */    
                     if(xReturn != pdPASS)
                     {
                        DBG("queue is full,reset\r\n");
                        xQueueReset(xTransKeyQueue);
                     }
                     else
                     {
                        DBG("door A  return\r\n");
                        check_msg_queue();
                     }                     
                }               
                                      

//                dbh("RECV A",(char *)buf,readLen);
                send_to_host(CONTROLMOTOR,buf,readLen);
                vTaskResume(xHandleTaskQueryMotor);//重启状态查询线程
                Motro_A = 0;
            }            
        }           

        
        /* 发送事件标志，表示任务正常运行 */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);
        
        vTaskDelay(100);
    }

}

#if 1
void vTaskKey(void *pvParameters)
{
    uint8_t CloseDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x01,0x8A,0x69 };
    uint8_t OpenDoor[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 };
//    uint8_t OpenDoor_R[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x03,0x0B,0xA8 };
//    uint8_t QuestStatus[8] =  { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
//    uint8_t MotorReset[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x07,0x0A,0x6B };    
	uint8_t ucKeyCode;

    BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
    uint32_t r_queue; /* 定义一个接收消息的变量 */
    uint8_t i = 0;

    uint8_t t1 = 0;
    uint8_t t2 = 0;

    static uint8_t isOpen = 1;
    
    while(1)
    {
		ucKeyCode = bsp_key_Scan(0);      
		
		if (ucKeyCode != KEY_NONE)
		{                
			switch (ucKeyCode)
			{
				/* 开门键按下执行向上位机发送开门请求 */
				case KEY_DOOR_B_PRES:
                    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");
					break;	
                case KEY_FIREFIGHTING_PRES:
//                    vTaskSuspend(xHandleTaskQueryMotor); //若是操作电机，则关掉电机查询 
                    mau_key = 100;
                    i = 0;
                    t1 = 0;
                    t2 =2;
                    do
                    {                           
                        xReturn = xQueueReceive( xTransKeyQueue,    /* 消息队列的句柄 */
                                                 &r_queue,      /* 发送的消息内容 */
                                                 (TickType_t)50); /* 等待时间 一直等 */
                        if(pdTRUE == xReturn&& i>0)
                        {
                            DBG("A第%d次接收 = %d\r\n",i,r_queue);
                            if(r_queue == 55)
                            {
                                t1 = 1;
                            }
                        }  
                        else
                        {
                            if(isOpen)
                            {
                                RS485_SendBuf(COM4,OpenDoor,8);//打开A门  
                            }
                            else
                            {
                                RS485_SendBuf(COM4,CloseDoor,8);//打开A门 
                            }
                        }
                        
                        xReturn = xQueueReceive( xTransMotorBQueue,    /* 消息队列的句柄 */
                                                    &r_queue,      /* 发送的消息内容 */
                                                    (TickType_t)50); /* 等待时间 一直等 */
                        if(pdTRUE == xReturn&& i>1)
                        {
                            DBG("B第%d次接收 = %d\r\n",i,r_queue);
                            if(r_queue == 66)
                            {
                                t2 = 1;
                            }
                        }
                        else
                        {
                            if(isOpen)
                            {
                                RS485_SendBuf(COM5,OpenDoor,8);//打开A门  
                            }
                            else
                            {
                                RS485_SendBuf(COM5,CloseDoor,8);//打开A门 
                            }
                        }

                        vTaskDelay(100);

                        if(t1==1 && t2==1)
                        {
                            mau_key = 0;
                            i=20;
                        }

                       check_msg_queue();
                    }
                    while (i++ < 8);
                    isOpen = !isOpen;
                    DBG("isOpen = %d\r\n",isOpen);                   
                    xQueueReset(xTransKeyQueue);
                    xQueueReset(xTransMotorBQueue);
                    //向android发送消防联动的消息
                    SendAsciiCodeToHost(FIREFIGHTINGLINKAGE,NO_ERR,"Fire fighting linkage");                    
                    break;
                case KEY_OPEN_DOOR_A_PRES:
//                    vTaskSuspend(xHandleTaskQueryMotor); //若是操作电机，则关掉电机查询
                    //Open door a manually
                    mau_key = 55;
                    i = 0;
                    do
                    {  
                        xReturn = xQueueReceive( xTransKeyQueue,    /* 消息队列的句柄 */
                                                 &r_queue,      /* 发送的消息内容 */
                                                 (TickType_t)50); /* 等待时间 一直等 */
                        if(pdTRUE == xReturn&& i!=0)
                        {
                            DBG("第%d次接收 = %d\r\n",i,r_queue);
                            if(r_queue == 55)
                            {
                                
                                mau_key = 0;
                                i = 20;
                            }
                        }  
                        else
                        {
                            RS485_SendBuf(COM4,OpenDoor,8);//打开A门                             
                            DBG("开A门\r\n");
                            vTaskDelay(100);   
                        }

                        check_msg_queue();
                    }
                    while (i++ < 8);                
                    xQueueReset(xTransKeyQueue);
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_A,NO_ERR,"Open door A manually"); 
                    break;
                case KEY_OPEN_DOOR_B_PRES:
//                    vTaskSuspend(xHandleTaskQueryMotor); //若是操作电机，则关掉电机查询
                    //Open door b manually
                    mau_key = 66;
                    i= 0;
                    do
                    {                        
                        xReturn = xQueueReceive( xTransMotorBQueue,    /* 消息队列的句柄 */
                                                 &r_queue,      /* 发送的消息内容 */
                                                 (TickType_t)50); /* 等待时间 一直等 */
                        if(pdTRUE == xReturn && i!=0)
                        {
                            DBG("第%d次接收 = %d\r\n",i,r_queue);
                            if(r_queue == 66)
                            {
                                
                                mau_key = 0;
                                i=20;
                            }
                        }
                        else
                        {
                            RS485_SendBuf(COM5,OpenDoor,8);//打开B门 
                            DBG("开B门\r\n");
                            vTaskDelay(100);

                        }

                        check_msg_queue();
                    }
                    while (i++ < 8); 
										xQueueReset(xTransMotorBQueue);
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_B,NO_ERR,"Open door B manually");
                    break;			
				/* 其他的键值不处理 */
				default:   
				App_Printf("KEY_default\r\n");
					break;
			}
		}	

		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_7);  

        
		vTaskDelay(50);
	}  

}
#else

void vTaskKey(void *pvParameters)
{
    
	uint8_t ucKeyCode;
    
    while(1)
    {
		ucKeyCode = bsp_key_Scan(0);      
		
		if (ucKeyCode != KEY_NONE)
		{                
			switch (ucKeyCode)
			{
				/* 开门键按下执行向上位机发送开门请求 */
				case KEY_DOOR_B_PRES:	 
                    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");
					break;			
			
				/* 其他的键值不处理 */
				default:   
				App_Printf("KEY_default\r\n");
					break;
			}
		}	

		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_7);  

        
		vTaskDelay(50);
	}  

}

#endif

void vTaskMsgPro(void *pvParameters)
{
    while(1)
    {          
        deal_Serial_Parse();    
	    deal_rx_data();

		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_2);
        vTaskDelay(100);
    }
}


void vTaskInfrared(void *pvParameters)
{
    uint32_t code = 0;
    uint8_t dat[3] = {0};
    
    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptInfraredToHost; 
    ptInfraredToHost = &gQueueToHost;
    #endif
    
    while(1)
    {  
        code = bsp_infrared_scan();       

        if(code != ERR_INFRARED)
        {
            memset(dat,0x00,sizeof(dat));
            
            dat[0] = code>>16;    
            dat[1] = code>>8;
            dat[2] = code&0xff;
            code = 0;

            #ifdef USEQUEUE
            ptInfraredToHost->cmd = GETSENSOR;
            memcpy(ptInfraredToHost->data,dat,3);
            
			/* 使用消息队列实现指针变量的传递 */
			if(xQueueSend(xTransQueue,              /* 消息队列句柄 */
						 (void *) &ptInfraredToHost,   /* 发送结构体指针变量ptQueueToHost的地址 */
						 (TickType_t)10) != pdPASS )
			{
                DBG("向xTransQueue发送数据失败，即使等待了10个时钟节拍\r\n");                
            } 
            else
            {
                //DBG("向xTransQueue发送数据成功\r\n");   
				dbh("GETSENSOR",(char *)dat,3);
            }
            #endif
                
            send_to_host(GETSENSOR,dat,3);
        }

		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_3);    
        
        vTaskDelay(20);
    }
}

void vTaskRs485(void *pvParameters)
{
    uint8_t buf[8] = {0};
    uint8_t readLen = 0;
    uint16_t iCRC = 0;
    uint8_t crcBuf[2] = {0};

    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptInfraredToHost; 
    ptInfraredToHost = &gQueueToHost;
    #endif

    BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
    uint32_t motora = 66;    
    
    while (1)
    {
        readLen = RS485_Recv(COM5,buf,8);       

        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            { 
                #ifdef USEQUEUE
                ptInfraredToHost->cmd = DOOR_B;
                memcpy(ptInfraredToHost->data,buf,readLen);
            
    			/* 使用消息队列实现指针变量的传递 */
    			if(xQueueSend(xTransQueue,              /* 消息队列句柄 */
    						 (void *) &ptInfraredToHost,   /* 发送结构体指针变量ptQueueToHost的地址 */
    						 (TickType_t)10) != pdPASS )
    			{
                    DBG("向xTransQueue发送数据失败，即使等待了10个时钟节拍\r\n");                
                } 
                else
                {                 
    				dbh("DOOR_B",(char *)buf,readLen);
                }   
                #endif

                if(readLen == 8 && mau_key >= 66 )
                {
                    xReturn = xQueueSend( xTransMotorBQueue, /* 消息队列的句柄 */
                                        &motora,/* 发送的消息内容 */
                                        50 );        /* 等待时间 0 */    
                     if(xReturn != pdPASS)
                     {
                        DBG("queue is full,reset\r\n");
                        xQueueReset(xTransMotorBQueue);
                     }
                     else
                     {
                        DBG("door B  return\r\n");
                        check_msg_queue();
                     }
                }                  
            
                send_to_host(DOOR_B,buf,readLen);
                vTaskResume(xHandleTaskQueryMotor);//重启状态查询线程
                Motro_B = 0;
            }            
        }
        
		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
        
        vTaskDelay(100);
    }

}


void vTaskReader(void *pvParameters)
{ 
    uint32_t CardID = 0;
    uint8_t dat[4] = {0};
    
//    uint32_t FunState = 0;
//    char *IcReaderState;
    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptReaderToHost; 
    ptReaderToHost = &gQueueToHost;
    #endif
//    IcReaderState = ef_get_env("ICSTATE");
//    assert_param(IcReaderState);
//    FunState = atol(IcReaderState);
    
    while(1)
    {

//        if(FunState != 0x00)
        {
            CardID = bsp_WeiGenScanf();

            if(CardID != 0)
            {
                memset(dat,0x00,sizeof(dat));            
                
    			dat[0] = CardID>>24;
    			dat[1] = CardID>>16;
    			dat[2] = CardID>>8;
    			dat[3] = CardID&0XFF;    

                #ifdef USEQUEUE
                ptReaderToHost->cmd = WGREADER;
                memcpy(ptReaderToHost->data,dat,4);

    			/* 使用消息队列实现指针变量的传递 */
    			if(xQueueSend(xTransQueue,              /* 消息队列句柄 */
    						 (void *) &ptReaderToHost,   /* 发送结构体指针变量ptQueueToHost的地址 */
    						 (TickType_t)100) != pdPASS )
    			{
                    DBG("向xTransQueue发送数据失败，即使等待了100个时钟节拍\r\n");                
                } 
                else
                {
                    dbh("WGREADER",(char *)dat,4);
                }
                #endif
                
                send_to_host(WGREADER,dat,4);
            }  
        }


		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_4);        
        
        vTaskDelay(100);
        
    }

}   


void vTaskQR(void *pvParameters)
{ 
    uint8_t recv_buf[512] = {0};
    uint16_t len = 0;  
    
//    uint32_t FunState = 0;
//    char *QrCodeState;

//    QrCodeState = ef_get_env("QRSTATE");
//    assert_param(QrCodeState);
//    FunState = atol(QrCodeState);
    
    while(1)
    {
//       if(FunState != 0x00)
       {
           memset(recv_buf,0x00,sizeof(recv_buf));
           len = comRecvBuff(COM3,recv_buf,sizeof(recv_buf));

           if(len > 0  && recv_buf[len-1] == 0x0A && recv_buf[len-2] == 0x0D)
           {
                DBG("QR = %s\r\n",recv_buf);
                SendAsciiCodeToHost(QRREADER,NO_ERR,recv_buf);
           }
           else
           {
                comClearTxFifo(COM3);
           }
       }

		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_5);  
        vTaskDelay(500);        
    }
}   


void vTaskHandShake(void *pvParameters)
{
    uint32_t i_boot_times = NULL;
    char *c_old_boot_times, c_new_boot_times[12] = {0};
    uint8_t bcdbuf[6] = {0};

    /* get the boot count number from Env */
    c_old_boot_times = ef_get_env("boot_times");
    assert_param(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);
    
    /* boot count +1 */
    i_boot_times ++;

    /* interger to string */
    sprintf(c_new_boot_times,"%012ld", i_boot_times);
    
    /* set and store the boot count number to Env */
    ef_set_env("boot_times", c_new_boot_times);    

    asc2bcd(bcdbuf,(uint8_t *)c_new_boot_times , 12, 0);

    send_to_host(HANDSHAKE,bcdbuf,6);  
    
    vTaskDelete( NULL ); //删除自己


//    uint32_t i_boot_times = NULL;
//    char *c_old_boot_times, c_new_boot_times[12] = {0};
//    

//    g500usCount = 1000*10;

//    c_old_boot_times = ef_get_env("boot_times");

//    DBG("1.c_old_boot_times = %s\r\n",c_old_boot_times);

//    while(1)
//    {
//        if(g500usCount == 0)
//        {
//            break;
//        }
//        /* get the boot count number from Env */
//        c_old_boot_times = ef_get_env("boot_times");
//        assert_param(c_old_boot_times);
//        i_boot_times = atol(c_old_boot_times);
//        
//        /* boot count +1 */
//        i_boot_times ++;

//        /* interger to string */
//        sprintf(c_new_boot_times,"%012ld", i_boot_times);

//        /* set and store the boot count number to Env */
//        ef_set_env("boot_times", c_new_boot_times);   


//    }

//    c_old_boot_times = ef_get_env("boot_times");

//    DBG("2.c_old_boot_times = %s\r\n",c_old_boot_times);    

//    vTaskDelete( NULL ); //删除自己
}



/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
static void  App_Printf(char *format, ...)
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



#ifdef USEQUEUE
static void vTaskMonitor(void *pvParameters)
{
  BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
  QUEUE_TO_HOST_T *ptMsg;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置最大等待时间为200ms */  

  while (1)
  {
    xReturn = xQueueReceive( xTransQueue,    /* 消息队列的句柄 */
                             (void *)&ptMsg,  /*这里获取的是结构体的地址 */
                             xMaxBlockTime); /* 设置阻塞时间 */
    if(pdPASS == xReturn)
    {
//        DBG("ptMsg->cmd = %02x\r\n", ptMsg->cmd);
//        dbh("ptMsg->data ", (char *)ptMsg->data,QUEUE_BUF_LEN);

        switch (ptMsg->cmd)
        {
            case GETSENSOR:
                 DBG("红外数据\r\n");
                break;
             case CONTROLMOTOR:
                 DBG("A门电机数据\r\n");
                break;
            case DOOR_B:
                 DBG("B门电机数据\r\n");
                break;
            case WGREADER:
                 DBG("读卡器数据\r\n");
                break;            
            
        }
    }    

  }    
}
#endif

//查询Message_Queue队列中的总队列数量和剩余队列数量
static void check_msg_queue(void)
{
    
	u8 msgq_remain_size;	//消息队列剩余大小
    u8 msgq_total_size;     //消息队列总大小
    
    taskENTER_CRITICAL();   //进入临界区
    msgq_remain_size=uxQueueSpacesAvailable(xTransKeyQueue);//得到队列剩余大小
    msgq_total_size=uxQueueMessagesWaiting(xTransKeyQueue)+uxQueueSpacesAvailable(xTransKeyQueue);//得到队列总大小，总大小=使用+剩余的。
	DBG("Total Size = %d, Remain Size = %d\r\n",msgq_total_size,msgq_remain_size);	//显示DATA_Msg消息队列总的大小

    taskEXIT_CRITICAL();    //退出临界区
}





