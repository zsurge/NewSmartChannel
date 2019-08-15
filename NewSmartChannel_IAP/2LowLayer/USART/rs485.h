#ifndef __RS485_H
#define __RS485_H			 
#include "sys.h"	 	

//引脚定义
/*******************************************************/
#define USART2_CLK                         RCC_APB1Periph_USART2
#define USART2_GPIO_PORT                   GPIOA
#define USART2_GPIO_CLK                    RCC_AHB1Periph_GPIOA

#define USART2_RX_PIN                      GPIO_Pin_3
#define USART2_RX_AF                       GPIO_AF_USART2
#define USART2_RX_SOURCE                   GPIO_PinSource3

#define USART2_TX_PIN                      GPIO_Pin_2
#define USART2_TX_AF                       GPIO_AF_USART2
#define USART2_TX_SOURCE                   GPIO_PinSource2

#define RCC_RS485_TXEN                     RCC_AHB1Periph_GPIOE
#define PORT_RS485_TXEN                    GPIOE
#define PIN_RS485_TXEN                     GPIO_Pin_2

//#define RCC_RS485_TXEN                     RCC_AHB1Periph_GPIOG
//#define PORT_RS485_TXEN                    GPIOG
//#define PIN_RS485_TXEN                     GPIO_Pin_8

#define RS485_MAX_LEN                       255

	  	
extern uint8_t RS485_RX_BUF[RS485_MAX_LEN]; 		//接收缓冲,最大64个字节
extern uint8_t RS485_RX_CNT;   			//接收到的数据长度





//模式控制
#define RS485_TX_EN		PEout(2)	//485模式控制.0,接收;1,发送.


														 
void RS485_Init(uint32_t bound);
void RS485_Send_Data(uint8_t *buf,uint16_t len);
void RS485_Receive_Data(uint8_t *buf,uint16_t *len);		 
//uint16_t RS485_Receive_Data (uint8_t *buf,uint16_t len,uint32_t timeout_MilliSeconds);

#endif	   
















