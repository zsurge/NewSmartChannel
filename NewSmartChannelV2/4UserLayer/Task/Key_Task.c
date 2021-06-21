/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : Key_Task.c
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��2��25��
  ����޸�   :
  ��������   : ������������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��2��25��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "key_task.h"
#include "bsp_key.h"
#include "tool.h"
#include "comm.h"
#include "string.h"


/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define KEY_TASK_PRIO	    (tskIDLE_PRIORITY + 4)
#define KEY_STK_SIZE        (configMINIMAL_STACK_SIZE*6)

#define FIRSTDOOR_ISOPEN_YES    1
#define FIRSTDOOR_ISOPEN_NO     0

#define SECDOOR_ISOPEN_YES    1
#define SECDOOR_ISOPEN_NO     0

#define MOTOR_NO1        1
#define MOTOR_NO2        2

#define MAX_TIME_OUT    100


/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *keyTaskName = "vKeyTask";   

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskKey = NULL;

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskKey(void *pvParameters);
static void optDoor(uint8_t motorNo);




void CreateKeyTask(void)
{
    //����
    xTaskCreate((TaskFunction_t )vTaskKey,         
                (const char*    )keyTaskName,       
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )KEY_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskKey); 
}


#if 0
static void vTaskKey(void *pvParameters)
{ 
	uint8_t ucKeyCode;
	
//	uint8_t pcWriteBuffer[1024];
//    uint32_t g_memsize;  
//    int32_t iTime1, iTime2;

    
    while(1)
    {
        ucKeyCode = bsp_key_Scan(0);      
		
		if (ucKeyCode != KEY_NONE)
		{
            DBG("ucKeyCode = %d\r\n",ucKeyCode);
              
			switch (ucKeyCode)
			{
				/* K1������ ��ӡ����ִ����� */
				case KEY_DOOR_B_PRES:	
				case KEY_FIREFIGHTING_PRES:
                case KEY_OPEN_DOOR_B_PRES: 				
				case KEY_OPEN_DOOR_A_PRES: 
				    DBG("KEY_OPEN_DOOR_A_PRES is press\r\n");		
				    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");                   
					break;
				/* �����ļ�ֵ������ */
				default:   
				    DBG("KEY_default\r\n");
					break;
			}
		}
		

        /* �����¼���־����ʾ������������ */
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_7);
		
		vTaskDelay(30);
	}  

}

#endif


static void optDoor(uint8_t motorNo)
{
    MOTORCTRL_QUEUE_T *ptMotor; 
    
    uint8_t OpenDoor[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 }; 

    ptMotor = &gMotorCtrlQueue;
	/* ��ʼ���ṹ��ָ�� */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 	
    
    if(motorNo == MOTOR_NO1)
    {   
        ptMotor->cmd = CONTROLMOTOR_A;
        memcpy(ptMotor->data,OpenDoor,MOTORCTRL_QUEUE_BUF_LEN); 
    
        /* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
        if(xQueueSend(gxMotorCtrlQueue,             /* ��Ϣ���о�� */
                     (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
                     (TickType_t)30) != pdPASS )
        {
            xQueueReset(gxMotorCtrlQueue);            
        }   


    }

}


static void vTaskKey(void *pvParameters)
{
    int32_t iTime1, iTime2;

    while (1)
    {        
        switch (Key_Scan(GPIO_PORT_KEY, GPIO_PIN_KEY_DOOR_B))
        {
            case KEY_ON:    
                iTime1 = xTaskGetTickCount();   /* ���¿�ʼʱ�� */
                SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");    
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:  
                iTime2 = xTaskGetTickCount();	/* ���½���ʱ�� */

                if(iTime2 - iTime1 > MAX_TIME_OUT)
                {
                    SendAsciiCodeToHost(PRESSUP,NO_ERR,"the key press up");  
                }
                break;
            case KEY_ERROR:              
                break;
            default:
                break;
        }

        switch (Key_Scan(GPIO_PORT_KEY, GPIO_PIN_FIREFIGHTING))
        {
            case KEY_ON:       
                iTime1 = xTaskGetTickCount();   /* ���¿�ʼʱ�� */
                SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");    
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:   
                iTime2 = xTaskGetTickCount();	/* ���½���ʱ�� */
            
                if(iTime2 - iTime1 > MAX_TIME_OUT)
                {
                    SendAsciiCodeToHost(PRESSUP,NO_ERR,"the key press up");  
                }

                break;
            case KEY_ERROR:              
                break;
            default:
                break;

        }

        switch (Key_Scan(GPIO_PORT_OPEN_DOOR, GPIO_PIN_OPEN_DOOR_A))
        {
            case KEY_ON:     
                iTime1 = xTaskGetTickCount();   /* ���¿�ʼʱ�� */
                SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");    
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:    
                iTime2 = xTaskGetTickCount();	/* ���½���ʱ�� */
            
                if(iTime2 - iTime1 > MAX_TIME_OUT)
                {
                    SendAsciiCodeToHost(PRESSUP,NO_ERR,"the key press up");  
                }

                break;
            case KEY_ERROR:              
                break;
            default:
                break;

        }        

        switch (Key_Scan(GPIO_PORT_OPEN_DOOR, GPIO_PIN_OPEN_DOOR_B))
        {
            case KEY_ON:   
                iTime1 = xTaskGetTickCount();   /* ���¿�ʼʱ�� */
                SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");    
                break;            
            case KEY_HOLD:
                break;
            case KEY_OFF:    
                iTime2 = xTaskGetTickCount();	/* ���½���ʱ�� */
            
                if(iTime2 - iTime1 > MAX_TIME_OUT)
                {
                    SendAsciiCodeToHost(PRESSUP,NO_ERR,"the key press up");  
                }

                break;
            case KEY_ERROR:              
                break;
            default:
                break;

        }
        
        /* �����¼���־����ʾ������������ */
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_7);     
        vTaskDelay(30);
    }

}



