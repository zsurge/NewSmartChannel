/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : MotorCtrl_SecDoor_Task.c
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��4��28��
  ����޸�   :
  ��������   : �ڶ���������Ʋ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��4��28��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/


/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include"MotorCtrl_SecDoor_Task.h"
#include "bsp_uart_fifo.h"
#include "comm.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define MOTOR_SEC_TASK_PRIO		( tskIDLE_PRIORITY + 7)
#define MOTOR_SEC_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)


/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *MotorCtrlSecDoorTaskName = "vSecMotorTask";      

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMotorCtrlSecDoor = NULL;  

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskMotorCtrlSecDoor(void *pvParameters);



void CreateMotorCtrlSecDoorTask(void)
{
    //���������Ϣ��������
    xTaskCreate((TaskFunction_t )vTaskMotorCtrlSecDoor,     
                (const char*    )MotorCtrlSecDoorTaskName,   
                (uint16_t       )MOTOR_SEC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_SEC_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotorCtrlSecDoor);     

}

static void vTaskMotorCtrlSecDoor(void *pvParameters)
{  
    BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
//    uint32_t i = 0;
    
//    uint8_t resetMotor[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x06,0x08,0x0C,0x00,0x07,0x0A,0x6B };
    uint8_t ReadStatus[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
    FROMHOST_STRU rxFromHost;   

    MOTORCTRL_QUEUE_T *ptMotor; 
	/* ��ʼ���ṹ��ָ�� */
	ptMotor = &gSecMotorCtrlQueue;

    /* ���� */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    {   

        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
        xReturn = xQueueReceive( gxMotorSecDoorCtrlQueue,    /* ��Ϣ���еľ�� */
                                 (void *)&ptMotor,          /*�����ȡ���ǽṹ��ĵ�ַ */
                                 (TickType_t)30);          /* ��������ʱ�� */
        if(pdTRUE == xReturn)
        {
            //��Ϣ���ճɹ������ͽ��յ�����Ϣ
            dbh("recv from host and send to MB:",(char *)ptMotor->data, MOTORCTRL_QUEUE_BUF_LEN);
            RS485_SendBuf(COM5, ptMotor->data,MOTORCTRL_QUEUE_BUF_LEN);//����B���  

        }
        else
        {
//            DBG("Send B door query status\r\n");
            //����Ĭ�����ݰ�
            RS485_SendBuf(COM5, ReadStatus,MOTORCTRL_QUEUE_BUF_LEN);//��ѯB���״̬
            
        }                            

        vTaskDelay(60);

        if(deal_motor_Parse(COM5,&rxFromHost) != 0)
        {
            dbh("recv MB and send to host:", (char *)rxFromHost.rxBuff,rxFromHost.rxCnt);  
            send_to_host(CONTROLMOTOR_B,rxFromHost.rxBuff,rxFromHost.rxCnt);              
            Motro_B = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));
        }
        

        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
      
    }

}






