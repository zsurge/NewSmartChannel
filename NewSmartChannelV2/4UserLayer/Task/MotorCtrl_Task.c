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
#include "math.h"
//#include"Monitor_Task.h"
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

    uint8_t OpenDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 };
    uint8_t ReadStatus[8] = { 0x01,0x03,0x07,0x04,0x00,0x0A,0x85,0x78 };
    uint16_t crc_data = 0;
    uint8_t tmpBuf[8] = {0};
    uint8_t sendBuf[8] = {0};
    uint8_t direction = 0;
    uint8_t isEnable = ENABLE_SEND;
    uint32_t expectValue = 0;
    uint32_t currentValue = 0;
    float percentage = 0.0;


	uint32_t tmp_expectValue = 0;
    uint32_t tmp_currentValue = 0;
    
    FROMHOST_STRU rxFromHost;  
    MOTORCTRL_QUEUE_T *ptMotor; 
	/* ��ʼ���ṹ��ָ�� */
	ptMotor = &gMotorCtrlQueue;
    
    /* ���� */
    ptMotor->cmd = 0;
    ptMotor->len = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    { 
       memset(sendBuf,0x00,sizeof(sendBuf));
                
        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
        xReturn = xQueueReceive( gxMotorCtrlQueue,    /* ��Ϣ���еľ�� */
                                 (void *)&ptMotor,  /*�����ȡ���ǽṹ��ĵ�ַ */
                                 (TickType_t)30); /* ��������ʱ�� */
        if(pdTRUE == xReturn)
        {
            //��Ϣ���ճɹ������ͽ��յ�����Ϣ
            //dbh("recv from host and send to MA:",(char *)ptMotor->data, ptMotor->len);
            if(ptMotor->len >= 8)
            {
                direction = ptMotor->data[8];
                memcpy(sendBuf,ptMotor->data,8);
            }
            else
            {
                //direction = 0;
                memcpy(sendBuf,ptMotor->data,8);
            }
        }
        else
        {
            //����Ĭ�����ݰ�
            //RS485_SendBuf(COM4, ReadStatus,8);//��ѯA���״̬
            memcpy(sendBuf,ReadStatus,8);
        }

        //������ڷ������ʱ���ѹ��ŽǶȴ���50%ʱ���������⣬�����ţ������ر�
        if(isEnable == DISABLE_SEND && sendBuf[5] == 0x02 && sendBuf[1] == 0x06)
        {
		   send_to_host(CONTROLMOTOR_A,OpenDoor,8); 
           //RS485_SendBuf(COM4, ReadStatus,8);//��ѯA���״̬  
        }
        else
        {
           RS485_SendBuf(COM4, sendBuf,8);
        }          

		
		//dbh("send to MA:",(char *)sendBuf, 8);
       
        vTaskDelay(50);

        if(deal_motor_Parse(COM4,&rxFromHost) != 0)
        { 
            //dbh("recv MA and send to host:", (char*)rxFromHost.rxBuff,rxFromHost.rxCnt);	

			
			 tmp_currentValue = u8ToU32(rxFromHost.rxBuff[3],rxFromHost.rxBuff[4],rxFromHost.rxBuff[5],rxFromHost.rxBuff[6]);
			 tmp_expectValue = u8ToU32(rxFromHost.rxBuff[7],rxFromHost.rxBuff[8],rxFromHost.rxBuff[9],rxFromHost.rxBuff[10]);

			 DBG("currentValue = <%d>, expectValue = <%d>\r\n",tmp_currentValue,tmp_expectValue);
			
            
            if(rxFromHost.rxBuff[1] == 0x03 && rxFromHost.rxCnt== 25)
            {            
                //��Ϊ��ѯָ����ˣ������������´�����ݸ���λ����������λ���Ķ�
                memset(tmpBuf,0x00,sizeof(tmpBuf));
                tmpBuf[0] = 0x01;
                tmpBuf[1] = 0x03;
                tmpBuf[2] = 0x02;
                tmpBuf[3] = rxFromHost.rxBuff[19];
                tmpBuf[4] = rxFromHost.rxBuff[20];
                crc_data = CRC16_Modbus(tmpBuf, 5);                
                tmpBuf[5] = crc_data>>8;
                tmpBuf[6] = crc_data & 0xFF;                 
                send_to_host(CONTROLMOTOR_A,tmpBuf,7);
            }
            else
            {
                send_to_host(CONTROLMOTOR_A,rxFromHost.rxBuff,rxFromHost.rxCnt);
            }          
            
            
            //direction = 0x55 ��ʾ������=0xAA ��ʾ������
            if(direction == 0x55 && rxFromHost.rxCnt== 25) 
            {            
                currentValue = u8ToU32(rxFromHost.rxBuff[3],rxFromHost.rxBuff[4],rxFromHost.rxBuff[5],rxFromHost.rxBuff[6]);
                expectValue = u8ToU32(rxFromHost.rxBuff[7],rxFromHost.rxBuff[8],rxFromHost.rxBuff[9],rxFromHost.rxBuff[10]);

				currentValue = abs(currentValue);
				expectValue = abs(expectValue);
				
				
				if(expectValue == 0 && percentage == 0.0F)
				{
					percentage = currentValue *0.6F;
 				}				


                DBG("currentValue = %d, expectValue = %d,percentage = %f\r\n",currentValue,expectValue,percentage);
                
                //����ֵ���ñ�־λ
                if(currentValue <percentage)
                {
                    isEnable = DISABLE_SEND;
                }
                else 
                {
                    isEnable = ENABLE_SEND;
                }
            }


            //��λ������״̬
            if(rxFromHost.rxBuff[19] == 0x08)
            {
                direction = 0;
                isEnable = ENABLE_SEND;
				expectValue = 0.0F;
            }
            
            Motro_A = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));            
        }

        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);      
    }

}



