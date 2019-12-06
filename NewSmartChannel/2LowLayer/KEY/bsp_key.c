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

#if 0
//按键初始化函数
void bsp_key_Init ( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_ALL_KEY, ENABLE ); //使能GPIOE

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_DOOR_B|GPIO_PIN_FIREFIGHTING; //对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
	GPIO_Init ( GPIO_PORT_KEY, &GPIO_InitStructure ); //初始化GPIOE9


	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_OPEN_DOOR_A|GPIO_PIN_OPEN_DOOR_B; //对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
	GPIO_Init ( GPIO_PORT_OPEN_DOOR, &GPIO_InitStructure ); //初始化GPIOE9
	
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY_DOOR_B按下

u8 bsp_key_Scan ( u8 mode )
{
	static u8 key_up=1;//按键按松开标志
	if ( mode )
	{
		key_up=1;    //支持连按
	}
	if ( key_up&& (KEY_FIREFIGHTING==0 || KEY_DOOR_B==0 || KEY_OPEN_DOOR_A==0 || KEY_OPEN_DOOR_B==0 ) )
	{
		delay_ms ( 10 ); //去抖动
		
		key_up=0;
        
		if ( KEY_FIREFIGHTING == 0 )
		{
			return KEY_FIREFIGHTING_PRES;
		}
        else if(KEY_DOOR_B==0)
        {
            return KEY_DOOR_B_PRES;
        }
        else if(KEY_OPEN_DOOR_A==0)
        {
            return KEY_OPEN_DOOR_A_PRES;
        }
        else if(KEY_OPEN_DOOR_B==0)
        {
            return KEY_OPEN_DOOR_B_PRES;
        }        

	}
	else if (KEY_FIREFIGHTING==1 && KEY_DOOR_B==1 && KEY_OPEN_DOOR_A==1 && KEY_OPEN_DOOR_B==1 )
	{
		key_up=1;
	}
    
	 return KEY_NONE;// 无按键按下
}

#else

//按键初始化函数
void bsp_key_Init ( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOE, ENABLE ); //使能GPIOE

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_DOOR_B; //对应引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
	GPIO_Init ( GPIO_PORT_KEY, &GPIO_InitStructure ); //初始化GPIOE9	
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY_DOOR_B按下

u8 bsp_key_Scan ( u8 mode )
{
	static u8 key_up=1;//按键按松开标志
	if ( mode )
	{
		key_up=1;    //支持连按
	}
	if ( key_up&& ( KEY_DOOR_B==0 ) )
	{
		delay_ms ( 10 ); //去抖动
		
		key_up=0;
        
		if ( KEY_DOOR_B == 0 )
		{
			return KEY_DOOR_B_PRES;
		}

	}
	else if ( KEY_DOOR_B == 1 )
	{
		key_up=1;
	}
    
	 return KEY_NONE;// 无按键按下
}


#endif

