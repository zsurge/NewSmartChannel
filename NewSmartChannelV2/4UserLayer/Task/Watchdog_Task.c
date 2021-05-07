/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : Watchdog_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年2月27日
  最近修改   :
  功能描述   : 看门狗任务
  函数列表   :
  修改历史   :
  1.日    期   : 2020年2月27日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/
/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "Watchdog_Task.h"
#include "bsp_iwdg.h"
#include "tool.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define WATCHDOG_TASK_PRIO		( tskIDLE_PRIORITY + 10)
#define WATCHDOG_STK_SIZE 		(configMINIMAL_STACK_SIZE*2)

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *WatchDogTaskName = "vWatchDogTask";  

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskWatchDog = NULL;

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskWatchDog(void *pvParameters);

void CreateWatchDogTask(void)
{
    //看门狗
    xTaskCreate((TaskFunction_t )vTaskWatchDog,     
                (const char*    )WatchDogTaskName,   
                (uint16_t       )WATCHDOG_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )WATCHDOG_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskWatchDog);
}



static void vTaskWatchDog(void *pvParameters)
{
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* 最大延迟1000ms */   
    
    /* 
    
    设置好看门狗狗的分频系数和重装载值，就可以知道看门狗的喂狗时间（溢出时间）。怎么来分析呢？
首先，看门狗的时钟是32kHz（不准确，有的教材上按40kHz计算），这个时钟的意思，就是看门狗计数器每隔1/32毫秒，下降一个值。如果预分频系数prep设置为4，也就是64分频(4×2^(prep))，那么，分频后的看门狗时钟成为了0.5kHz，也就是，计数器每隔2ms变化一次。而计数器从多少开始计数呢？这取决于我们写入RLR重装载寄存器里的值是多少，假如我们写入rlr = 500，那么，它就经过1000ms，也就是1秒的时间，才能减到0。在这1秒中的时间间隔之内去喂狗，看门狗就不会复位。
综上分析，溢出时间Tout的计算公式为：Tout = 4 × 2^(prep) × rlr / 32[ms]。


    
      开始执行启动任务主函数前使能独立看门狗。
      设置LSI是32分频，下面函数参数范围0-0xFFF，分别代表最小值1ms和最大值4095ms
      下面设置的是4s，如果4s内没有喂狗，系统复位。
    */
//    bsp_InitIwdg(4000);
      bsp_InitIwdg(2500);//5S内没有喂狗就系统复位

    
    /* 打印系统开机状态，方便查看系统是否复位 */
    DBG("=====================================================\r\n");
    DBG("系统开机执行\r\n");
    DBG("=====================================================\r\n");
    
    while(1)
    {   
        
        /* 等待所有任务发来事件标志 */
        uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
                                     TASK_BIT_ALL,       /* 等待TASK_BIT_ALL被设置 */
                                     pdTRUE,             /* 退出前TASK_BIT_ALL被清除，这里是TASK_BIT_ALL都被设置才表示“退出”*/
                                     pdTRUE,             /* 设置为pdTRUE表示等待TASK_BIT_ALL都被设置*/
                                     xTicksToWait);      /* 等待延迟时间 */
        
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
            
        }

    }
}




