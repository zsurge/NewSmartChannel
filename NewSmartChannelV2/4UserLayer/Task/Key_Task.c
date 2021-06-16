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


/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *keyTaskName = "vKeyTask";     

static uint8_t isFirstOpen = FIRSTDOOR_ISOPEN_YES;
static uint8_t isSecondOpen = SECDOOR_ISOPEN_YES;

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
					DBG("KEY_DOOR_B_PRES is press\r\n");					
//	                iTime1 = xTaskGetTickCount();   /* ���¿�ʼʱ�� */
//	                iTime2 = xTaskGetTickCount();   /* ���½���ʱ�� */
//	                DBG ( "�޸ļ�¼�ɹ�����ʱ: %dms\r\n",iTime2 - iTime1 );
	                
//					printf("=================================================\r\n");
//					printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
//					vTaskList((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);
//                    
//					printf("\r\n������       ���м���         ʹ����\r\n");
//					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);          

//                    g_memsize = xPortGetFreeHeapSize();
//                    printf("ϵͳ��ǰ�ڴ��СΪ %d �ֽڣ���ʼ�����ڴ�\n",g_memsize);
                    
//                    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");
                    KeyOpenDoorB();
					break;				
				/* K2�����£���ӡ���ڲ������� */
				case KEY_FIREFIGHTING_PRES:
				    DBG("KEY_FIREFIGHTING_PRES is press\r\n");
                    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");
                    optDoor(MOTOR_NO1);
				    optDoor(MOTOR_NO2);
					break;
				case KEY_OPEN_DOOR_A_PRES: 
				    DBG("KEY_OPEN_DOOR_A_PRES is press\r\n");				    
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_A,NO_ERR,"Open door A manually"); 
                    optDoor(MOTOR_NO1);
					break;
				case KEY_OPEN_DOOR_B_PRES: 
				    DBG("KEY_OPEN_DOOR_B_PRES is press\r\n");                    
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_B,NO_ERR,"Open door B manually");
                    optDoor(MOTOR_NO2);
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


static void optDoor(uint8_t motorNo)
{
    MOTORCTRL_QUEUE_T *ptMotor; 
    
    uint8_t OpenDoor[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 }; 
    uint8_t CloseDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x01,0x8A,0x69 };
    
    ptMotor = &gMotorCtrlQueue;
	/* ��ʼ���ṹ��ָ�� */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 	    


//    DBG("motorNo = %d,isFirstOpen = %d,isSecondOpen = %d\r\n",motorNo,isFirstOpen,isSecondOpen);
    
    if(motorNo == MOTOR_NO1)
    {   
        ptMotor->cmd = CONTROLMOTOR_A;
        
//        if(isFirstOpen)
//        {
            memcpy(ptMotor->data,OpenDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }
//        else
//        {
//            memcpy(ptMotor->data,CloseDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }

//        isFirstOpen = !isFirstOpen;
                
    
        /* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
        if(xQueueSend(gxMotorCtrlQueue,             /* ��Ϣ���о�� */
                     (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
                     (TickType_t)30) != pdPASS )
        {
            xQueueReset(gxMotorCtrlQueue);            
        }   


    }
    else
    {
        ptMotor->cmd = CONTROLMOTOR_B;
        
//        if(isSecondOpen)
//        {
            memcpy(ptMotor->data,OpenDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }
//        else
//        {
//            memcpy(ptMotor->data,CloseDoor,MOTORCTRL_QUEUE_BUF_LEN); 
//        }

//        
//        isSecondOpen = !isSecondOpen;
        
        /* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
        if(xQueueSend(gxMotorSecDoorCtrlQueue,      /* ��Ϣ���о�� */
                     (void *) &ptMotor,             /* ���ͽṹ��ָ�����ptReader�ĵ�ַ */
                     (TickType_t)30) != pdPASS )
        {
            xQueueReset(gxMotorSecDoorCtrlQueue);
        }


    }
}



