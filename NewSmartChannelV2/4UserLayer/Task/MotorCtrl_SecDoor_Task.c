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

    uint8_t ReadStatus[8] = { 0x01,0x03,0x07,0x04,0x00,0x0A,0x85,0x78 };
    uint16_t crc_data = 0;
    uint8_t tmpBuf[8] = {0};
    uint8_t sendBuf[8] = {0};
    uint8_t direction = 0;
    uint8_t isEnable = ENABLE_SEND;
    uint32_t expectValue = 0;
    uint32_t currentValue = 0;
    float percentage = 0.0;

    FROMHOST_STRU rxFromHost;
    MOTORCTRL_QUEUE_T *ptMotor; 
    
	/* ��ʼ���ṹ��ָ�� */
	ptMotor = &gSecMotorCtrlQueue;

    /* ���� */
    ptMotor->cmd = 0;
    ptMotor->len = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    {   
        memset(sendBuf,0x00,sizeof(sendBuf));

        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
        xReturn = xQueueReceive( gxMotorSecDoorCtrlQueue,    /* ��Ϣ���еľ�� */
                                 (void *)&ptMotor,          /*�����ȡ���ǽṹ��ĵ�ַ */
                                 (TickType_t)30);          /* ��������ʱ�� */
        if(pdTRUE == xReturn)
        {
            //��Ϣ���ճɹ������ͽ��յ�����Ϣ
            dbh("recv from host and send to MB:",(char *)ptMotor->data, ptMotor->len);
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
//            DBG("Send B door query status\r\n");
            //����Ĭ�����ݰ�
            memcpy(sendBuf,ReadStatus,8);
        }      

        //������ڷ������ʱ���ѹ��ŽǶȴ���50%ʱ���������⣬�����ţ������ر�
        if(isEnable == DISABLE_SEND && sendBuf[5] == 0x02 && sendBuf[1] == 0x06)
        {
           RS485_SendBuf(COM5, ReadStatus,8);//��ѯB���״̬  
        }
        else
        {
           RS485_SendBuf(COM5, sendBuf,8);
        } 
        

        vTaskDelay(60);

        if(deal_motor_Parse(COM5,&rxFromHost) != 0)
        {
            dbh("recv MB and send to host:", (char *)rxFromHost.rxBuff,rxFromHost.rxCnt);  

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
                send_to_host(CONTROLMOTOR_B,tmpBuf,7);
            }
            else
            {
                send_to_host(CONTROLMOTOR_B,rxFromHost.rxBuff,rxFromHost.rxCnt);
            } 

            //direction = 0x55 ��ʾ������=0xAA ��ʾ������
            if(direction == 0x55 && rxFromHost.rxCnt== 25) 
            {            
                currentValue = u8ToU32(rxFromHost.rxBuff[3],rxFromHost.rxBuff[4],rxFromHost.rxBuff[5],rxFromHost.rxBuff[6]);
                expectValue = u8ToU32(rxFromHost.rxBuff[7],rxFromHost.rxBuff[8],rxFromHost.rxBuff[9],rxFromHost.rxBuff[10]);

                percentage = currentValue/expectValue;

                DBG("currentValue = %d, expectValue = %d,percentage = %f\r\n",currentValue,expectValue,percentage);
                
                //����ֵ���ñ�־λ
                if(percentage > 0.5f)
                {
                    isEnable = DISABLE_SEND;
                }
                else
                {
                    isEnable = ENABLE_SEND;
                }
            }
//            else
//            {
//                isEnable = ENABLE_SEND;
//            }

            //��λ������״̬
            if(rxFromHost.rxBuff[19] == 0x08)
            {
                direction = 0;
                isEnable = ENABLE_SEND;
            }            
            
            Motro_B = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));
        }
        

        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
      
    }

}






