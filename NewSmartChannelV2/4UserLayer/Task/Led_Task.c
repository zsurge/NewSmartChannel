/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : Led_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年2月25日
  最近修改   :
  功能描述   : LED灯控制
  函数列表   :
  修改历史   :
  1.日    期   : 2020年2月25日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "led_task.h"
#include "tool.h"
#include "bsp_led.h" 
#include "comm.h"
#include "bsp_beep.h" 
#include "bsp_uart_fifo.h"
#include "Devinfo.h"




/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define LED_TASK_PRIO	    ( tskIDLE_PRIORITY+5)
#define LED_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *ledTaskName = "vLedTask";      //看门狗任务名


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskLed = NULL;      //LED灯


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskLed(void *pvParameters);

static void DisplayDevInfo (void);

static void DisplayDevInfo(void)
{
	printf("Softversion :%s\r\n",gDevinfo.SoftwareVersion);
    printf("HardwareVersion :%s\r\n", gDevinfo.HardwareVersion);
	printf("Model :%s\r\n", gDevinfo.Model);
	printf("ProductBatch :%s\r\n", gDevinfo.ProductBatch);	    
	printf("BulidDate :%s\r\n", gDevinfo.BulidDate);
	printf("DevSn :%s\r\n", gDevinfo.GetSn());
}


//static void showTask ( void );

//static void showTask ( void )
//{
//	uint8_t pcWriteBuffer[1024] = {0};

//	printf ( "=================================================\r\n" );
//	printf ( "任务名      任务状态 优先级   剩余栈 任务序号\r\n" );
//	vTaskList ( ( char* ) &pcWriteBuffer );
//	printf ( "%s\r\n", pcWriteBuffer );

//	printf ( "\r\n任务名       运行计数         使用率\r\n" );
//	vTaskGetRunTimeStats ( ( char* ) &pcWriteBuffer );
//	printf ( "%s\r\n", pcWriteBuffer );
//	printf ( "当前动态内存剩余大小 = %d字节\r\n", xPortGetFreeHeapSize() );
//}

void CreateLedTask(void)
{
    //创建LED任务
    xTaskCreate((TaskFunction_t )vTaskLed,         
                (const char*    )ledTaskName,       
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )LED_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskLed);   

}


//LED任务函数 
static void vTaskLed(void *pvParameters)
{   
    uint16_t i = 0;    
    uint8_t bcdbuf[6] = {0x00,0x00,0x00,0x01,0x02,0x03};
    
    LED_L_G = 1;
    LED_R_G = 1;
    DisplayDevInfo();

    //send_to_host(HANDSHAKE,bcdbuf,6);  

       
    BEEP = 0;
    vTaskDelay(300);    
    BEEP = 1;
    
    while(1)
    {          
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
            
//            showTask();
        }

//        //获取任务通知，等待100个时间节拍，获取到，则执行上位机指令，获取不到，则执行状态查询
//		xReturn=xTaskNotifyWait(0x0,			//进入函数的时候不清除任务bit
//                            ULONG_MAX,	        //退出函数的时候清除所有的bit
//                            (uint32_t *)&recvbuff,//保存任务通知值                    
//                            (TickType_t)xMaxBlockTime);	//阻塞时间
//                            
//        if( pdTRUE == xReturn )
//        {            
//            memcpy(tmp,recvbuff,MAX_EXLED_LEN);      

//                bsp_Ex_SetLed((uint8_t*)tmp); 
//                respondLed();                
//                gTime2 = xTaskGetTickCount();
//                DBG("set led use %d ms\r\n",gTime2 - gTime1);
//        }         
        
		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_0);  
        vTaskDelay(100);     
    }
}   


