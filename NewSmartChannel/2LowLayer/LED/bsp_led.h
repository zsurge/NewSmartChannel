#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "sys.h"


//LED端口定义
#define RCC_ALL_LED     (RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOE)

#define GPIO_PORT_LED1      GPIOF
#define GPIO_PORT_LED23     GPIOE
#define GPIO_PORT_LED4      GPIOB


#define GPIO_PIN_LED1    GPIO_Pin_14
#define GPIO_PIN_LED2    GPIO_Pin_10
#define GPIO_PIN_LED3    GPIO_Pin_11
#define GPIO_PIN_LED4    GPIO_Pin_11




//通道指示灯定义
#define RCC_EX_LED     (RCC_AHB1Periph_GPIOF)

#define GPIO_PORT_EX_LED    GPIOF

#define GPIO_PIN_L_R    GPIO_Pin_7     //左侧红灯
#define GPIO_PIN_L_G    GPIO_Pin_6     //左侧绿灯
#define GPIO_PIN_M_R    GPIO_Pin_9     //中间红灯
#define GPIO_PIN_M_G    GPIO_Pin_8     //中间绿灯
#define GPIO_PIN_R_R    GPIO_Pin_5     //右侧红灯
#define GPIO_PIN_R_G    GPIO_Pin_4     //右侧绿灯


#define LED_L_R    PFout(7)      //左侧红灯
#define LED_L_G    PFout(6)     //左侧绿灯
#define LED_M_R    PFout(9)     //中间红灯
#define LED_M_G    PFout(8)     //中间绿灯
#define LED_R_R    PFout(5)     //右侧红灯
#define LED_R_G    PFout(4)     //右侧绿灯


#define LED1 PFout(14)	//
#define LED2 PEout(10)	// 
#define LED3 PEout(11)	// 
#define LED4 PBout(11)	// 	





void bsp_LED_Init(void);//初始化		 	
void bsp_Ex_LED_Init(void); //扩展FLASH初始化
void bsp_Ex_SetLed(uint8_t *dat);

//void bsp_test_Init(void);





//void bsp_LedToggle(uint8_t _no);
#endif
