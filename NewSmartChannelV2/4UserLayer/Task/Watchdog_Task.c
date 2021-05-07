/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : Watchdog_Task.c
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��2��27��
  ����޸�   :
  ��������   : ���Ź�����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��2��27��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/
/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "Watchdog_Task.h"
#include "bsp_iwdg.h"
#include "tool.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define WATCHDOG_TASK_PRIO		( tskIDLE_PRIORITY + 10)
#define WATCHDOG_STK_SIZE 		(configMINIMAL_STACK_SIZE*2)

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *WatchDogTaskName = "vWatchDogTask";  

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskWatchDog = NULL;

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskWatchDog(void *pvParameters);

void CreateWatchDogTask(void)
{
    //���Ź�
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
    const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* ����ӳ�1000ms */   
    
    /* 
    
    ���úÿ��Ź����ķ�Ƶϵ������װ��ֵ���Ϳ���֪�����Ź���ι��ʱ�䣨���ʱ�䣩����ô�������أ�
���ȣ����Ź���ʱ����32kHz����׼ȷ���еĽ̲��ϰ�40kHz���㣩�����ʱ�ӵ���˼�����ǿ��Ź�������ÿ��1/32���룬�½�һ��ֵ�����Ԥ��Ƶϵ��prep����Ϊ4��Ҳ����64��Ƶ(4��2^(prep))����ô����Ƶ��Ŀ��Ź�ʱ�ӳ�Ϊ��0.5kHz��Ҳ���ǣ�������ÿ��2ms�仯һ�Ρ����������Ӷ��ٿ�ʼ�����أ���ȡ��������д��RLR��װ�ؼĴ������ֵ�Ƕ��٣���������д��rlr = 500����ô�����;���1000ms��Ҳ����1���ʱ�䣬���ܼ���0������1���е�ʱ����֮��ȥι�������Ź��Ͳ��Ḵλ��
���Ϸ��������ʱ��Tout�ļ��㹫ʽΪ��Tout = 4 �� 2^(prep) �� rlr / 32[ms]��


    
      ��ʼִ����������������ǰʹ�ܶ������Ź���
      ����LSI��32��Ƶ�����溯��������Χ0-0xFFF���ֱ������Сֵ1ms�����ֵ4095ms
      �������õ���4s�����4s��û��ι����ϵͳ��λ��
    */
//    bsp_InitIwdg(4000);
      bsp_InitIwdg(2500);//5S��û��ι����ϵͳ��λ

    
    /* ��ӡϵͳ����״̬������鿴ϵͳ�Ƿ�λ */
    DBG("=====================================================\r\n");
    DBG("ϵͳ����ִ��\r\n");
    DBG("=====================================================\r\n");
    
    while(1)
    {   
        
        /* �ȴ������������¼���־ */
        uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* �¼���־���� */
                                     TASK_BIT_ALL,       /* �ȴ�TASK_BIT_ALL������ */
                                     pdTRUE,             /* �˳�ǰTASK_BIT_ALL�������������TASK_BIT_ALL�������òű�ʾ���˳���*/
                                     pdTRUE,             /* ����ΪpdTRUE��ʾ�ȴ�TASK_BIT_ALL��������*/
                                     xTicksToWait);      /* �ȴ��ӳ�ʱ�� */
        
        if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
        {            
            IWDG_Feed(); //ι��    
        }
        else
            
        
        {
            /* ������ÿxTicksToWait����һ�� */
            /* ͨ������uxBits�򵥵Ŀ����ڴ˴�����Ǹ�������û�з������б�־ */

            //ʱ��ԭ��ֵ��̫׼ȷ����Ҫ����׼�ķ���

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




