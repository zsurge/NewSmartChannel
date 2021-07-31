/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : comm.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��6��18��
  ����޸�   :
  ��������   : ����ͨѶЭ�����������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��6��18��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __COMM_H
#define __COMM_H

#include "tool.h"
#include "cJSON.h"
#include "bsp_infrared.h"
#include "errorcode.h"
#include "bsp_sensor.h"

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define STX		    					0x02	/* frame head */
#define ETX                             0x03    /* frame tail */
#define CMDSUCC	    					0x06	/* command success */
#define CMDERRO	    					0x15	/* command error */
#define UPDAT							0x13	/* Upload data */
    
    
#define SPACE		        			0x00
#define FINISH		       	 			0x55

#define GETSENSOR                       0xA1
#define SETLED                          0xA2
#define GETDEVICEINFO                   0xA3
#define GETVER                          0xA4
#define DEVRESET                        0xA5
#define SETDEVPARAM                     0xA6
#define UPGRADE                         0xA7
#define CONTROLMOTOR_A                  0xA8

#define WGREADER                        0xA9
#define QRREADER                        0xA0

#define PRESSUP                         0xAA    //�����뿪
#define SHOWINFORMATION_STATIC          0x20    //����LED��ʾ��̬��Ϣ
#define SHOWINFORMATION                 0x21    //����LED��ʾ������Ϣ

#define SETLEDVALUE                     0x22
#define SETLEDVALUE_END                 0x23


#define REQUEST_OPEN_DOOR_B             0xB7
#define CONTROLMOTOR_B                  0xB8

#define ERRORINFO                       0XB0

#define HANDSHAKE                       0XB1

#define FIREFIGHTINGLINKAGE             0xB2
#define MANUALLY_OPEN_DOOR_A            0xB3
#define MANUALLY_OPEN_DOOR_B            0xB4

//add 2020.05.26
#define SET_MOTOR_A_PARAM               0xB5
#define SET_MOTOR_B_PARAM               0xB6



#define DEV_MOTOR                       0x01
#define DEV_SENSOR                      0x02
#define DEV_HEARTBEAT                   0x03



#define JSON_PACK_MAX                   600

#define MAX_RXD_BUF_LEN        			512
#define MAX_TXD_BUF_LEN					512   
#define MAX_CMD_BUF_LEN					512  
#define MAX_CMD_BCD_BUF_LEN				256  

#define MAX_HOST_CMD_LEN			    256 
#define MAX_CMD_DESC_LEN			    20 

#define MAX_EXLED_LEN                   0x0F

//typedef enum
//{
//    NoCMD = 0xA0,
//    GetSensor = 0xA1,
//    SetLed,
//    GetDeviceStatus,
//    GetVersion,
//    UpGradeApp    
//}CommandType;    

typedef struct
{
    uint8_t cmd;     //ָ����
    uint8_t cmd_desc[MAX_CMD_DESC_LEN];//ָ������    
    uint8_t len;
    uint8_t cmd_data[MAX_CMD_BUF_LEN];//ָ������(����)
}CMD_RX_T;

//static CMD_RX_T gcmd_rx;

typedef struct
{
    uint8_t cmd;         //ָ����
    uint8_t code;        //ִ��״̬   
    uint8_t data[MAX_CMD_BUF_LEN];       //ָ�������(����)
}CMD_TX_T;

typedef struct
{
    uint8_t RxdStatus;                 //����״̬
    uint8_t RxCRCHi;                   //У��ֵ��8λ
    uint8_t RxCRCLo;                   //У��ֵ��8λ
    uint8_t RxdFrameStatus;            //���հ�״̬
    uint16_t NowLen;                    //�����ֽ���
    uint16_t RxdTotalLen;               //���հ����ݳ���
    volatile uint8_t RxdBuf[MAX_HOST_CMD_LEN];   //���հ����ݻ���
}RECVHOST_T;

static RECVHOST_T gRecvHost;



/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
void deal_rx_data(void);
void deal_Serial_Parse(void);

void init_serial_boot(void);
SYSERRORCODE_E send_to_host(uint8_t cmd,uint8_t *buf,uint8_t len);
void send_to_device(CMD_RX_T *cmd_rx);
SYSERRORCODE_E SendAsciiCodeToHost(uint8_t cmd,SYSERRORCODE_E code,uint8_t *buf);

void respondLed(void);
void KeyOpenDoorB(void);
//void respHost(uint8_t *cmd,uint8_t len);

char send_to_host_queue(uint8_t *buf,int len);

#endif

