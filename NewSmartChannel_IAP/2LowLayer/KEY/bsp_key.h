/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : key.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月25日
  最近修改   :
  功能描述   : 键盘驱动
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月25日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __BSP_KEY_H
#define __BSP_KEY_H	 

#include "sys.h" 
#include "delay.h"


/* 按键口对应的RCC时钟 */
#define RCC_ALL_KEY     (RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC)

#define GPIO_PORT_KEY       GPIOA

#define GPIO_PIN_KEY_SET    GPIO_Pin_4
#define GPIO_PIN_KEY_RR     GPIO_Pin_0
#define GPIO_PIN_KEY_LL     GPIO_Pin_1

#define GPIO_PORT_KEY_OK    GPIOC
#define GPIO_PIN_KEY_OK     GPIO_Pin_3


#define KEY_SET     PAin(4)   	//PA4
#define KEY_RR 		PAin(0)		//PA0 
#define KEY_LL 		PAin(1)		//PA1
#define KEY_OK		PCin(3)		//PC3 

//#define KEY_SET 	GPIO_ReadInputDataBit(GPIO_PORT_KEY,GPIO_PIN_KEY_SET)   //PA4
//#define KEY_RR 		GPIO_ReadInputDataBit(GPIO_PORT_KEY,GPIO_PIN_KEY_RR)	//PA0  
//#define KEY_LL 		GPIO_ReadInputDataBit(GPIO_PORT_KEY,GPIO_PIN_KEY_LL)    //PA1
//#define KEY_OK 	    GPIO_ReadInputDataBit(GPIO_PORT_KEY_OK,GPIO_PIN_KEY_OK)	//PC3 


#define KEY_NONE        0   //无按键
#define KEY_SET_PRES 	1	//KEY0按下
#define KEY_RR_PRES	    2	//KEY1按下
#define KEY_LL_PRES	    3	//KEY2按下
#define KEY_OK_PRES     4	//KEY3按下


void bsp_key_Init(void);	//IO初始化
u8 bsp_key_Scan(u8);  		//按键扫描函数		


#endif

