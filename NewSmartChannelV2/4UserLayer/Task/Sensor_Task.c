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
#include "tool.h"
#include "BSP_Uart.h"

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
    uint8_t dat[4] = {0};
    uint8_t asc[8] = {0};

    uint8_t sensorValue[43] = { 0x02,0x00,0x29,0x7b,0x22,0x63,0x6d,0x64,0x22,0x3a,0x22,0x61,0x31,0x22,0x2c,0x22,0x63,0x6f,0x64,0x65,0x22,0x3a,0x30,0x2c,0x22,0x64,0x61,0x74,0x61,0x22,0x3a,0x22,0x30,0x30,0x30,0x30,0x30,0x38,0x22,0x7d,0x03,0xa5,0xa5 };
    
    while(1)
    {  
        code = bsp_sensor_scan();
        
        if(code != ERR_INFRARED)
        {
            memset(dat,0x00,sizeof(dat));
            
            dat[0] = code>>16;    
            dat[1] = code>>8;
            dat[2] = code&0xff;
            code = 0;
            
            bcd2asc(asc, dat, 6, 1);
            
            memcpy(sensorValue+32,asc,6);            

            //if(xSemaphoreTake(gxMutex, portMAX_DELAY))
            //{
                BSP_UartSend(SCOM1,sensorValue,43); 
            //}
            
            //xSemaphoreGive(gxMutex);            
        }


        //�����¼���־����ʾ������������        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_3);            
        vTaskDelay(30);   
    }
}


