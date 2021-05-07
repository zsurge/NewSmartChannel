/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : bsp_dma_usart1.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��22��
  ����޸�   :
  ��������   : ����1��ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __BSP_DMA_USART1_H
#define __BSP_DMA_USART1_H
#include "stdio.h"	
#include "string.h"
#include "stdint.h"
#include "ringbuff.h"


//���Ŷ���
/*******************************************************/
#define USART1_CLK                         RCC_APB2Periph_USART1
#define USART1_GPIO_PORT                   GPIOB
#define USART1_GPIO_CLK                    RCC_AHB1Periph_GPIOB

#define USART1_RX_PIN                      GPIO_Pin_7
#define USART1_RX_AF                       GPIO_AF_USART1
#define USART1_RX_SOURCE                   GPIO_PinSource7

#define USART1_TX_PIN                      GPIO_Pin_6
#define USART1_TX_AF                       GPIO_AF_USART1
#define USART1_TX_SOURCE                   GPIO_PinSource6


#define COM1_BUFF_SIZE   512 //���ڽ��ջ���������
#define UART1_RX_LEN         COM1_BUFF_SIZE //USART6 DMA���ջ���������
#define UART1_TX_LEN         COM1_BUFF_SIZE //USART6 DMA���ͻ���������


extern uint8_t gUsart1RXBuff[COM1_BUFF_SIZE];//���ջ�����
extern RingBuff_t ringbuff_handle;


void bsp_Usart1_Init(uint32_t bound);
void bsp_DMAUsart1Send(uint8_t *str,uint8_t cndtr);
uint16_t bsp_DMAUsart1Read(uint8_t *buff, uint32_t len);
    
#endif


