/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : key.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月25日
  最近修改   :
  功能描述   : 全高门(B门)开门按键
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月25日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __bsp_KEY_H
#define __bsp_KEY_H	 

#include "sys.h" 
#include "delay.h"

#define KEY_DOOR_B          PEin(9) 
#define KEY_FIREFIGHTING    PEin(2)

#define KEY_OPEN_DOOR_A     PDin(7)
#define KEY_OPEN_DOOR_B     PDin(4)


/* 按键口对应的RCC时钟 */
#define RCC_ALL_KEY             (RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)

#define GPIO_PORT_KEY           GPIOE
#define GPIO_PIN_KEY_DOOR_B     GPIO_Pin_9
#define GPIO_PIN_FIREFIGHTING    GPIO_Pin_2


#define GPIO_PORT_OPEN_DOOR    GPIOD
#define GPIO_PIN_OPEN_DOOR_A     GPIO_Pin_7
#define GPIO_PIN_OPEN_DOOR_B     GPIO_Pin_4
  	



#define KEY_NONE                0   //无按键
#define KEY_DOOR_B_PRES 	    1	//B门开门键按下
#define KEY_FIREFIGHTING_PRES 	2	//消防联动键按下
#define KEY_OPEN_DOOR_A_PRES 	3	//A门开门键按下
#define KEY_OPEN_DOOR_B_PRES 	4	//B门开门键按下


//宏定义
#define    	KEY_OFF	   		1
#define    	KEY_ON	   	 	0
#define    	KEY_HOLD		2
#define		KEY_IDLE		3
#define		KEY_ERROR		10

#define		HOLD_COUNTS			50
#define 	SHAKES_COUNTS		5


//按键状态结构体，存储四个变量
typedef struct
{
 	uint8_t KeyLogic;
	uint8_t KeyPhysic;
 	uint8_t KeyONCounts;
 	uint8_t KeyOFFCounts;
}KEY_TypeDef;







void bsp_key_Init(void);	//IO初始化
//uint8_t bsp_key_Scan(uint8_t);  		//按键扫描函数		
uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);


#endif

