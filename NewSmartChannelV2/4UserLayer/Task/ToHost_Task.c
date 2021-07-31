/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "ToHost_Task.h"
#include "string.h"
//#include "bsp_dma_usart1.h"
#include "BSP_Uart.h"
#include "log.h"
#include "tool.h"
#include "malloc.h"


/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define TOHOST_TASK_PRIO		(tskIDLE_PRIORITY + 12)
#define TOHOST_STK_SIZE 		(configMINIMAL_STACK_SIZE*4)

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *ToHostTaskName = "vToHostTask";      

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskToHost = NULL;  

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskToHost(void *pvParameters);


void CreateToHostTask(void)
{
    //��Ϣ���д�����λ��
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
        
        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
        xReturn = xQueueReceive( gxToHostQueue,    /* ��Ϣ���еľ�� */
                                 (void *)&send,    /*�����ȡ���ǽṹ��ĵ�ַ */
                                 (TickType_t)20);  /* ��������ʱ�� */
        if(pdTRUE == xReturn)
        { 
            //dbh("send to host",(char *)send->data,send->len);
            
            if(send->len != 0)
            {                
                //��Ϣ���ճɹ������ͽ��յ�����Ϣ
//                bsp_DMAUsart1Send(send->data,send->len);     
                BSP_UartSend(SCOM1,send->data,send->len); 
            }
            else
            {
                DBG("vTaskToHost data error\r\n");
            }
        }

      
    }
}

