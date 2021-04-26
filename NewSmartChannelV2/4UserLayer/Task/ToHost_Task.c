/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "ToHost_Task.h"
#include "string.h"
#include "bsp_uart.h"
#include "log.h"
#include "tool.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define TOHOST_TASK_PRIO		(tskIDLE_PRIORITY + 9)
#define TOHOST_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)

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
        send->cmd = 0;
        send->len = 0;
        memset(send->data,0x00,sizeof(send->data));
        
        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
        xReturn = xQueueReceive( gxToHostQueue,    /* ��Ϣ���еľ�� */
                                 (void *)&send,    /*�����ȡ���ǽṹ��ĵ�ַ */
                                 (TickType_t)30);  /* ��������ʱ�� */
        if(pdTRUE == xReturn)
        {            
            //��Ϣ���ճɹ������ͽ��յ�����Ϣ
            BSP_UartSend(SCOM1,send->data,send->len); 
            dbh("recv data",(char *)send->data,send->len);
        }
 

        /* �����¼���־����ʾ������������ */        
        //xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);

        
        vTaskDelay(20);
      
    }
}


