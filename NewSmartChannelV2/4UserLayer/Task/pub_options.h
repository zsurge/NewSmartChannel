/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : pub_options.h
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��2��25��
  ����޸�   :
  ��������   : FreeRTOS���¼���֪ͨ�ȹ��������Ķ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��2��25��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __PUB_OPTIONS_H__
#define __PUB_OPTIONS_H__

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define ULONG_MAX 0xffffffffUL

//�¼���־
#define TASK_BIT_0	 (1 << 0)
#define TASK_BIT_1	 (1 << 1)
#define TASK_BIT_2	 (1 << 2)
#define TASK_BIT_3	 (1 << 3)
#define TASK_BIT_4	 (1 << 4)
#define TASK_BIT_5	 (1 << 5)
#define TASK_BIT_6	 (1 << 6)
#define TASK_BIT_7	 (1 << 7)
#define TASK_BIT_ALL ( TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 |TASK_BIT_4|TASK_BIT_5|TASK_BIT_7)

//��ȡ���״̬������
//#define READ_MOTOR_STATUS_TIMES 40


#define MOTORCTRL_QUEUE_LEN    20     /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define MOTORCTRL_QUEUE_BUF_LEN 8     //ÿ������buff�ĳ���

#define TOHOST_DATA_MAX_LEN    512    //ͨ�����д����������ݳ���

typedef struct
{
    uint8_t cmd;                                    //ָ����
    uint8_t data[MOTORCTRL_QUEUE_BUF_LEN];          //��Ҫ���͸�android�������
}MOTORCTRL_QUEUE_T;

typedef struct
{ 
    uint8_t data[TOHOST_DATA_MAX_LEN];              //��Ҫ���͸�android�������
    uint16_t len;
}TOHOST_QUEUE_T;


/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/ 
//�¼����
extern EventGroupHandle_t xCreatedEventGroup;
extern SemaphoreHandle_t gxMutex;
extern QueueHandle_t gxMotorCtrlQueue; 
extern QueueHandle_t gxMotorSecDoorCtrlQueue; 
extern QueueHandle_t gxToHostQueue;



extern MOTORCTRL_QUEUE_T gMotorCtrlQueue;    //����һ���ṹ��������Ϣ���У���andoridͨ��
extern MOTORCTRL_QUEUE_T gSecMotorCtrlQueue;
extern TOHOST_QUEUE_T    gToHostQueueBuf,gToHostQueueBuf_recv;


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/



#endif /* __PUB_OPTIONS_H__ */

