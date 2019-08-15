#include "bsp_led.h" 
#include "string.h"
 

//初始化PF9和PF10为输出口.并使能这两个口的时钟		    
//LED IO初始化
void bsp_LED_Init(void)
{    	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_ALL_LED, ENABLE);//使能GPIOF时钟
    
      //GPIOF1
      GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1 ;   //LED1对应IO口
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //普通输出模式
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
      GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);         //初始化GPIO
    
      //GPIOF23
      GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2|GPIO_PIN_LED3 ; //LED2和LED3对应IO口
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //普通输出模式
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
      GPIO_Init(GPIO_PORT_LED23, &GPIO_InitStructure);            //初始化GPIO
    
      //GPIOF4
      GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED4 ;   //LED4
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //普通输出模式
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;            //上拉
      GPIO_Init(GPIO_PORT_LED4, &GPIO_InitStructure);         //初始化GPIO        
    
    
      GPIO_SetBits(GPIO_PORT_LED1,GPIO_PIN_LED1 );            
      GPIO_SetBits(GPIO_PORT_LED23,GPIO_PIN_LED2|GPIO_PIN_LED3);
      GPIO_SetBits(GPIO_PORT_LED4,GPIO_PIN_LED4);



}


void bsp_Ex_LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_EX_LED, ENABLE);	//使能GPIOF时钟

	//GPIOG9,D2初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_L_G|GPIO_PIN_L_R|GPIO_PIN_M_R|GPIO_PIN_M_G|GPIO_PIN_R_R|GPIO_PIN_R_G;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(GPIO_PORT_EX_LED, &GPIO_InitStructure);			    //初始化GPIO


	GPIO_ResetBits(GPIO_PORT_EX_LED,GPIO_PIN_L_G|GPIO_PIN_M_R|GPIO_PIN_M_G|GPIO_PIN_R_R|GPIO_PIN_R_G );	//灯灭

}


void bsp_Ex_SetLed(uint8_t *dat)
{
    uint8_t buf[64] = {0};
    
    memcpy(buf,dat,15);    

    if(buf[0] == 0x4C && buf[5] == 0x4D && buf[10] == 0x52)
    {
        LED_L_R = buf[2];
        LED_L_G = buf[4];
        LED_M_R = buf[7];
        LED_M_G = buf[9];
        LED_R_R = buf[12];
        LED_R_G = buf[14];
    }    
    
}


//void bsp_test_Init(void)
//{
//    GPIO_InitTypeDef  GPIO_InitStructure;
//    
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟 
//    
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟


//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;   
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //普通输出模式
//      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
//      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
//      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
//      GPIO_Init(GPIOD, &GPIO_InitStructure);         //初始化GPIO

//      

//      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;   
//      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //普通输出模式
//      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
//      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
//      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
//      GPIO_Init(GPIOA, &GPIO_InitStructure);         //初始化GPIO


//      GPIO_ResetBits(GPIOA,GPIO_Pin_1); 
//      GPIO_ResetBits(GPIOD,GPIO_Pin_3);

//      
//}


