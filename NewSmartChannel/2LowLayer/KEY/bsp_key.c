/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : key.c
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

#include "bsp_key.h"


//按键初始化函数
void bsp_key_Init ( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_ALL_KEY, ENABLE ); //使能GPIOA,GPIOC时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_RR|GPIO_PIN_KEY_LL|GPIO_PIN_KEY_SET; //KEY0 KEY1 KEY2对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
	GPIO_Init ( GPIO_PORT_KEY, &GPIO_InitStructure ); //初始化GPIOE2,3,4


	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_OK;//WK_UP对应引脚PA0
	GPIO_Init ( GPIO_PORT_KEY_OK, &GPIO_InitStructure ); //初始化GPIOA0
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下
//4，KEY_UP按下 即WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY_UP!!
u8 bsp_key_Scan ( u8 mode )
{
	static u8 key_up=1;//按键按松开标志
	if ( mode )
	{
		key_up=1;    //支持连按
	}
	if ( key_up&& ( KEY_SET==0||KEY_RR==0||KEY_LL==0||KEY_OK==0 ) )
	{
		delay_ms ( 10 ); //去抖动
		
		key_up=0;
        
		if ( KEY_SET==0 )
		{
			return KEY_SET_PRES;
		}
		else if ( KEY_RR==0 )
		{
			return KEY_RR_PRES;
		}
		else if ( KEY_LL==0 )
		{
			return KEY_LL_PRES;
		}
		else if ( KEY_OK==0 )
		{
			return KEY_OK_PRES;
		}
	}
	else if ( KEY_SET==1&&KEY_RR==1&&KEY_LL==1&&KEY_OK==1 )
	{
		key_up=1;
	}
    
	 return KEY_NONE;// 无按键按下
}


