/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : Sensor_Task.c
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��2��27��
  ����޸�   :
  ��������   : ���⴫�����źŴ�������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��2��27��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "Sensor_Task.h"
#include "bsp_infrared.h"
#include "bsp_sensor.h"
#include "comm.h"
#include "string.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define SENSOR_TASK_PRIO		(tskIDLE_PRIORITY + 1)
#define SENSOR_STK_SIZE 		(configMINIMAL_STACK_SIZE*4)

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *SensorTaskName = "vSensorTask";      

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskSensor = NULL;  

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskSensor(void *pvParameters);


void CreateSensorTask(void)
{
    //���⴫����״̬����
    xTaskCreate((TaskFunction_t )vTaskSensor,     
                (const char*    )SensorTaskName,   
                (uint16_t       )SENSOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )SENSOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskSensor);  
}


static void vTaskSensor(void *pvParameters)
{
    uint32_t code = 0;
    uint8_t dat[3+1] = {0};
    
    while(1)
    {  
        code = bsp_sensor_scan();//bsp_infrared_scan();// ;       


        if(code != ERR_INFRARED)
        {
            memset(dat,0x00,sizeof(dat));
            
            dat[0] = code>>16;    
            dat[1] = code>>8;
            dat[2] = code&0xff;
            code = 0;
                
            send_to_host(GETSENSOR,dat,3);
        }


        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_3);    
        
        vTaskDelay(10);
    }
}


