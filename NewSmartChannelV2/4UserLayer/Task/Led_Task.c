/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : Led_Task.c
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��2��25��
  ����޸�   :
  ��������   : LED�ƿ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��2��25��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "led_task.h"
#include "tool.h"
#include "bsp_led.h" 
#include "comm.h"
#include "bsp_beep.h" 
#include "bsp_uart_fifo.h"
#include "Devinfo.h"





/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define LED_TASK_PRIO	    ( tskIDLE_PRIORITY+5)
#define LED_STK_SIZE 		(configMINIMAL_STACK_SIZE)

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *ledTaskName = "vLedTask";      //���Ź�������


/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskLed = NULL;      //LED��


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskLed(void *pvParameters);
//static void showTask ( void );

//static void showTask ( void )
//{
//	uint8_t pcWriteBuffer[1024] = {0};

//	printf ( "=================================================\r\n" );
//	printf ( "������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n" );
//	vTaskList ( ( char* ) &pcWriteBuffer );
//	printf ( "%s\r\n", pcWriteBuffer );

//	printf ( "\r\n������       ���м���         ʹ����\r\n" );
//	vTaskGetRunTimeStats ( ( char* ) &pcWriteBuffer );
//	printf ( "%s\r\n", pcWriteBuffer );
//	printf ( "��ǰ��̬�ڴ�ʣ���С = %d�ֽ�\r\n", xPortGetFreeHeapSize() );
//}

void CreateLedTask(void)
{
    //����LED����
    xTaskCreate((TaskFunction_t )vTaskLed,         
                (const char*    )ledTaskName,       
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )LED_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskLed);   

}


//LED������ 
static void vTaskLed(void *pvParameters)
{   
    uint16_t i = 0;    
    
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

        //ϵͳ״̬���еƣ�ÿ500ms һ��
        i++;
        if(i == 5)
        {
            i = 0;
            LED4=!LED4; 
            
//            showTask();
        }

//        //��ȡ����֪ͨ���ȴ�100��ʱ����ģ���ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
//		xReturn=xTaskNotifyWait(0x0,			//���뺯����ʱ���������bit
//                            ULONG_MAX,	        //�˳�������ʱ��������е�bit
//                            (uint32_t *)&recvbuff,//��������ֵ֪ͨ                    
//                            (TickType_t)xMaxBlockTime);	//����ʱ��
//                            
//        if( pdTRUE == xReturn )
//        {            
//            memcpy(tmp,recvbuff,MAX_EXLED_LEN);      

//                bsp_Ex_SetLed((uint8_t*)tmp); 
//                respondLed();                
//                gTime2 = xTaskGetTickCount();
//                DBG("set led use %d ms\r\n",gTime2 - gTime1);
//        }         
        
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_0);  
        vTaskDelay(100);     
    }
}   


