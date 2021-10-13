/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : Version.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��8��21��
  ����޸�   :
  ��������   : ʵ�ְ汾����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��8��21��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "Devinfo.h"
#include "tool.h"
#include "string.h"
#include "stdio.h"
/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/


/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static uint8_t * GetDevSn(void);

const uint8_t Softversion[] = "1.1.0";
const uint8_t Hardversion[] = "1.0.1";
const uint8_t SoftBulidDate[] = __DATE__ " " __TIME__;
const uint8_t DevModel[] = "SmartChannelAPP";
const uint8_t DevBatch[] = "20201013001";


static uint8_t * GetDevSn(void)
{ 
    static unsigned int chipid[3] = { 0 };
    
    static unsigned char id[30] = {0};
    
    static unsigned char id1[10] = { 0 };
    static unsigned char id2[10] = { 0 };
    static unsigned char id3[10] = { 0 };

    chipid[0] = *(__I uint32_t *)(0x1FFF7A10 + 0x00);
    chipid[1] = *(__I uint32_t *)(0x1FFF7A10 + 0x04);
    chipid[2] = *(__I uint32_t *)(0x1FFF7A10 + 0x08); 

    int2Str(id1, chipid[0]);
	int2Str(id2, chipid[1]);
	int2Str(id3, chipid[2]);

    strcpy((char *)id,(char *)id1);
    strcat((char *)id,(char *)id2);
    strcat((char *)id,(char *)id3);

    return id;  
}

const DEVICEINFO_T gDevinfo = {Softversion, Hardversion,SoftBulidDate,DevModel,DevBatch,GetDevSn};




//static void DisplayDevInfo (void);

//static void DisplayDevInfo(void)
//{
//	printf("Softversion :%s\r\n",gDevinfo.SoftwareVersion);
//    printf("HardwareVersion :%s\r\n", gDevinfo.HardwareVersion);
//	printf("Model :%s\r\n", gDevinfo.Model);
//	printf("ProductBatch :%s\r\n", gDevinfo.ProductBatch);	    
//	printf("BulidDate :%s\r\n", gDevinfo.BulidDate);
//	printf("DevSn :%s\r\n", gDevinfo.GetSn());
//}


