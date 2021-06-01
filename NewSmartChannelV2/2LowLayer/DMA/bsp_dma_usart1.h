/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_dma_usart1.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月22日
  最近修改   :
  功能描述   : 串口1初始化
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月22日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

#ifndef __BSP_DMA_USART1_H
#define __BSP_DMA_USART1_H
#include "stdio.h"	
#include "string.h"
#include "stdint.h"
#include "ringbuff.h"


//引脚定义
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


#define USART1_DMA_MAX_SIZE 512 

extern volatile uint8_t USART1_DMA_RecvBuf[USART1_DMA_MAX_SIZE];
extern volatile uint16_t RecvTop1;
extern volatile uint16_t RecvEnd1;



void bsp_Usart1_Init(uint32_t bound);
void bsp_DMAUsart1Send(uint8_t *str,uint8_t cndtr);
uint16_t bsp_DMAUsart1Read(uint8_t *buff, uint32_t len);
uint8_t bsp_DMAUsart1ReadOne (uint8_t *Str);   
#endif


