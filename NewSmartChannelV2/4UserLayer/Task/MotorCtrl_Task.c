/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : MotorCtrl_Task.c
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��2��26��
  ����޸�   :
  ��������   : �����������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��2��26��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include"MotorCtrl_Task.h"
#include "bsp_uart_fifo.h"
#include "comm.h"
#include "string.h"
#include "bsp_led.h"


/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 6)
#define MOTOR_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)



/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *MotorCtrlTaskName = "vMotorCtrlTask";      

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMotorCtrl = NULL;     




/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskMotorCtrl(void *pvParameters);



void CreateMotorCtrlTask(void)
{
    //���������Ϣ��������
    xTaskCreate((TaskFunction_t )vTaskMotorCtrl,     
                (const char*    )MotorCtrlTaskName,   
                (uint16_t       )MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotorCtrl);     

}

static void vTaskMotorCtrl(void *pvParameters)
{  
    BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
//    uint32_t i = 0;
//    uint8_t buf[8] = {0};
//    uint16_t readLen = 0;
//    uint16_t iCRC = 0;
//    uint8_t crcBuf[2] = {0};
//    uint8_t CloseDoor[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x06,0x08,0x0C,0x00,0x01,0x8A,0x69 };
    uint8_t ReadStatus[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
//    uint8_t resetMotor[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x06,0x08,0x0C,0x00,0x07,0x0A,0x6B };
//    uint32_t NotifyValue = 0x55;
    FROMHOST_STRU rxFromHost;   
    MOTORCTRL_QUEUE_T *ptMotor; 
	/* ��ʼ���ṹ��ָ�� */
	ptMotor = &gMotorCtrlQueue;
    
    /* ���� */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    { 
        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
        xReturn = xQueueReceive( gxMotorCtrlQueue,    /* ��Ϣ���еľ�� */
                                 (void *)&ptMotor,  /*�����ȡ���ǽṹ��ĵ�ַ */
                                 (TickType_t)30); /* ��������ʱ�� */
        if(pdTRUE == xReturn)
        {
            //��Ϣ���ճɹ������ͽ��յ�����Ϣ
            dbh("recv from host and send to MA:",(char *)ptMotor->data, MOTORCTRL_QUEUE_BUF_LEN);
            RS485_SendBuf(COM4, ptMotor->data,MOTORCTRL_QUEUE_BUF_LEN);//����A���
    
//   ȫ��    
//            r     g
//    L 4C   6C 01 67 00   
//    M 4D   6C 01 67 00
//    R 52   6C 01 67 00


//            if(ptMotor->data[5]==0x02)
//            {
//                //���ţ��ɳ����������ڷ�����
//                LED_L_R = 1;
//                LED_L_G = 0;
//                LED_R_R = 0;
//                LED_R_G = 1;                
//            }
        }
        else
        {
            //����Ĭ�����ݰ�
            RS485_SendBuf(COM4, ReadStatus,MOTORCTRL_QUEUE_BUF_LEN);//��ѯA���״̬
        }  
        

        vTaskDelay(100);

        if(deal_motor_Parse(COM4,&rxFromHost) != 0)
        { 
            dbh("recv MA and send to host:", (char*)rxFromHost.rxBuff,rxFromHost.rxCnt); 
            send_to_host(CONTROLMOTOR_A,rxFromHost.rxBuff,rxFromHost.rxCnt);   

//            if(rxFromHost.rxBuff[3] == 0x08)
//            {               
//                //���ŵ�λ
//                LED_L_R = 1;
//                LED_L_G = 0;
//                LED_R_R = 1;
//                LED_R_G = 0;       
//                LED_M_R = 1;
//                LED_M_G = 0;                  
//            }
            
            Motro_A = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));            
        }

        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);      
    }

}



