/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "ToHost_Task.h"
#include "string.h"
#include "bsp_dma_usart1.h"
#include "log.h"
#include "tool.h"
#include "malloc.h"


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define TOHOST_TASK_PRIO		(tskIDLE_PRIORITY + 9)
#define TOHOST_STK_SIZE 		(configMINIMAL_STACK_SIZE*4)

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
        send->len = 0;
        memset(send->data,0x00,sizeof(send->data));
        
        //获取到，则执行上位机指令，获取不到，则执行状态查询
        xReturn = xQueueReceive( gxToHostQueue,    /* 消息队列的句柄 */
                                 (void *)&send,    /*这里获取的是结构体的地址 */
                                 (TickType_t)30);  /* 设置阻塞时间 */
        if(pdTRUE == xReturn)
        { 
            DBG("======vTaskDataProcess mem perused = %3d%======\r\n",mem_perused(SRAMIN));
            dbh("send to host",(char *)send->data,send->len);
            
            if(send->len != 0)
            {                
                //消息接收成功，发送接收到的消息
                bsp_DMAUsart1Send(send->data,send->len);     
            }
            else
            {
                DBG("vTaskToHost data error\r\n");
            }
        }

      
    }
}


