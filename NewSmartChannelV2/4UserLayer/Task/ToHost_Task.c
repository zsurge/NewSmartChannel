/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "ToHost_Task.h"
#include "string.h"
#include "bsp_uart.h"
#include "log.h"
#include "tool.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define TOHOST_TASK_PRIO		(tskIDLE_PRIORITY + 9)
#define TOHOST_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *ToHostTaskName = "vToHostTask";      

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskToHost = NULL;  

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskToHost(void *pvParameters);


void CreateToHostTask(void)
{
    //消息队列传给上位机
    xTaskCreate((TaskFunction_t )vTaskToHost,     
                (const char*    )ToHostTaskName,   
                (uint16_t       )TOHOST_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TOHOST_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskToHost);  
}


static void vTaskToHost(void *pvParameters)
{
    BaseType_t xReturn = pdTRUE;
    
    TOHOST_QUEUE_T *send= &gToHostQueueBuf_recv; 

    while (1)
    { 
        send->cmd = 0;
        send->len = 0;
        memset(send->data,0x00,sizeof(send->data));
        
        //获取到，则执行上位机指令，获取不到，则执行状态查询
        xReturn = xQueueReceive( gxToHostQueue,    /* 消息队列的句柄 */
                                 (void *)&send,    /*这里获取的是结构体的地址 */
                                 (TickType_t)30);  /* 设置阻塞时间 */
        if(pdTRUE == xReturn)
        {            
            //消息接收成功，发送接收到的消息
            BSP_UartSend(SCOM1,send->data,send->len); 
            dbh("recv data",(char *)send->data,send->len);
        }
 

        /* 发送事件标志，表示任务正常运行 */        
        //xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);

        
        vTaskDelay(20);
      
    }
}


