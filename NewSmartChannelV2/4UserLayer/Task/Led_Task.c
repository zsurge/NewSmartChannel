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
#include "malloc.h"



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

LED_VALUE_STRU gLedValueQueue;

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
    BaseType_t xReturn = pdTRUE;
//    LED_VALUE_STRU *ptLedValue = &gLedValueQueue; 
    
    BEEP = 0;
    vTaskDelay(300);    
    BEEP = 1;
    
    while(1)
    {  
//        ptLedValue->cmd = 0;
//        ptLedValue->type = 0;
//        memset(ptLedValue->data,0x00,SETLED_QUEUE_BUF_LEN);
        
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

        //ϵͳ״̬���еƣ�ÿ100ms һ��
        LED4=!LED4;         


//        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
//        xReturn = xQueueReceive( gxLedSetQueue,    /* ��Ϣ���еľ�� */
//                                 (void *)&ptLedValue,  /*�����ȡ���ǽṹ��ĵ�ַ */
//                                 (TickType_t)30); /* ��������ʱ�� */
//        if(pdTRUE == xReturn)
//        {
//            if(ptLedValue->cmd == 0x22) //��ʼ��˸
//            {  
//                
//            }
//            else if(ptLedValue->cmd == 0x23) //�ر���˸����ʾ�̵�
//            {    
//                
//            } 
//        }
                    
        
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_0);  
        vTaskDelay(50);     
    }
}   


