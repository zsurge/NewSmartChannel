/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_infrared.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年6月28日
  最近修改   :
  功能描述   : 处理红外传感器信号
  函数列表   :
  修改历史   :
  1.日    期   : 2019年6月28日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "bsp_infrared.h"
#include "stdio.h"
#include "string.h"
#include "tool.h"
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
void bsp_infrared_init ( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_ALL_SENSOR, ENABLE );	//使能GPIOF时钟


	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SENSOR1|GPIO_PIN_SENSOR2|GPIO_PIN_SENSOR3|GPIO_PIN_SENSOR4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//普通输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		    //上拉
	GPIO_Init ( GPIO_PORT_SENSOR14, &GPIO_InitStructure );		//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SENSOR5 |GPIO_PIN_SENSOR6;	    //LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//普通输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		    //上拉
	GPIO_Init ( GPIO_PORT_SENSOR56, &GPIO_InitStructure );		//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_SENSOR7 |GPIO_PIN_SENSOR8;	    //LED0和LED1对应IO口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//普通输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		    //上拉
	GPIO_Init ( GPIO_PORT_SENSOR78, &GPIO_InitStructure );		//初始化GPIO    

	GPIO_SetBits ( GPIO_PORT_SENSOR14,GPIO_PIN_SENSOR1|GPIO_PIN_SENSOR2|GPIO_PIN_SENSOR3|GPIO_PIN_SENSOR4);
	GPIO_SetBits ( GPIO_PORT_SENSOR56,GPIO_PIN_SENSOR5 |GPIO_PIN_SENSOR6 );
	GPIO_SetBits ( GPIO_PORT_SENSOR78,GPIO_PIN_SENSOR7 |GPIO_PIN_SENSOR8 );


    
}

uint32_t bsp_infrared_scan ( void )
{
    static uint32_t iret = 0;
	static u8 key_up=1;//按键按松开标志

	
	static u8 key1_up=0;
	static u8 key2_up=0;
	static u8 key3_up=0;
	static u8 key4_up=0;
	static u8 key5_up=0;
	static u8 key6_up=0;
	static u8 key7_up=0;
	static u8 key8_up=0;

	static u8 key9_up=0;
	static u8 key10_up=0;
	static u8 key11_up=0;
	static u8 key12_up=0;
	static u8 key13_up=0;
	static u8 key14_up=0;
	static u8 key15_up=0;
	static u8 key16_up=0;

	static u8 key17_up=0;
	static u8 key18_up=0;
	static u8 key19_up=0;
	static u8 key20_up=0;
	static u8 key21_up=0;
	static u8 key22_up=0;
	static u8 key23_up=0;
	static u8 key24_up=0;    
    
	u8 mode = 0;
    
	if ( mode )
	{
		key_up=1;    //支持连按
	}

	if ( key_up && ( GETS1==0||GETS2==0 || GETS3==0||GETS4==0 || GETS5==0||GETS6==0 || GETS7==0||GETS8==0 \
                    |GETS9==0||GETS10==0 || GETS11==0||GETS12==0 || GETS13==0||GETS14==0 || GETS15==0||GETS16==0 \
                    |GETS17==0||GETS18==0 || GETS19==0||GETS20==0 || GETS21==0||GETS22==0 || GETS23==0||GETS24==0) )
	{
		delay_ms ( 5 ); //去抖动

		key_up=0;
        
		if ( GETS1==0 && key1_up==0)
		{
            key1_up = 1;            
			return iret |= (0x01<<0);
		}
		else if ( GETS2==0 && key2_up==0)
		{
            key2_up = 1;
			return iret |= (0x01<<1);
		}
		else if ( GETS3==0 && key3_up==0)
		{
            key3_up = 1;
			return iret |= (0x01<<2);
		}
		else if ( GETS4==0 && key4_up==0)
		{
            key4_up = 1;
			return iret |= (0x01<<3);
		}
		else if ( GETS5==0 && key5_up==0)
		{
            key5_up = 1;
			return iret |= (0x01<<4);
		}
		else if ( GETS6==0 && key6_up==0)
		{
            key6_up = 1;
			return iret |= (0x01<<5);
		}
		else if ( GETS7==0 && key7_up==0)
		{
            key7_up = 1;
			return iret |= (0x01<<6);
		}
		else if ( GETS8==0 && key8_up==0)
		{
            key8_up = 1;
			return iret |= (0x01<<7);
		}
		else if ( GETS9==0 && key9_up==0)
		{
            key9_up = 1;
			return iret |= (0x01<<8);
		}    
		else if ( GETS10==0 && key10_up==0)
		{
            key10_up = 1;
			return iret |= (0x01<<9);
		}
		else if ( GETS11==0 && key11_up==0)
		{
            key11_up = 1;
			return iret |= (0x01<<10);
		}   
		else if ( GETS12==0 && key12_up==0)
		{
            key12_up = 1;
			return iret |= (0x01<<11);
		}
		else if ( GETS13==0 && key13_up==0)
		{
            key13_up = 1;
			return iret |= (0x01<<12);
		}  
		else if ( GETS14==0 && key14_up==0)
		{
            key14_up = 1;
			return iret |= (0x01<<13);
		}
		else if ( GETS15==0 && key15_up==0)
		{
            key15_up = 1;
			return iret |= (0x01<<14);
		}  
		else if ( GETS16==0 && key16_up==0)
		{
            key16_up = 1;
			return iret |= (0x01<<15);
		}
		else if ( GETS17==0 && key17_up==0)
		{
            key17_up = 1;
			return iret |= (0x01<<16);
		}  
		else if ( GETS18==0 && key18_up==0)
		{
            key18_up = 1;
			return iret |= (0x01<<17);
		}
		else if ( GETS19==0 && key19_up==0)
		{
            key19_up = 1;
			return iret |= (0x01<<18);
		}    
		else if ( GETS20==0 && key20_up==0)
		{
            key20_up = 1;
			return iret |= (0x01<<19);
		}
		else if ( GETS21==0 && key21_up==0)
		{
            key21_up = 1;
			return iret |= (0x01<<20);
		}  
		else if ( GETS22==0 && key22_up==0)
		{
            key22_up = 1;
			return iret |= (0x01<<21);
		}
		else if ( GETS23==0 && key23_up==0)
		{
            key23_up = 1;
			return iret |= (0x01<<22);
		}  
		else if ( GETS24==0 && key24_up==0)
		{
            key24_up = 1;
			return iret |= (0x01<<23);
		}
  
        

	}
//	else if ((key_up==0) && (GETS1==1 || GETS2==1 || GETS3==1 || GETS4==1 || GETS5==1 || GETS6==1 || GETS7==1 || GETS8==1))
    else if ( (key_up==0)&&( GETS1==1||GETS2==1||GETS3==1||GETS4==1||GETS5==1||GETS6==1||GETS7==1||GETS8==1 \
            |GETS9==1||GETS10==1||GETS11==1||GETS12==1||GETS13==1||GETS14==1||GETS15==1||GETS16==1 \
            |GETS17==1||GETS18==1||GETS19==1||GETS20==1||GETS21==1||GETS22==1||GETS23==1||GETS24==1))
	{        
        delay_ms ( 5 ); //去抖动       
		key_up=1;     
        
		if ( GETS1==1 && key1_up==1)
		{
            key1_up = 0;
			return iret &= ~(0x01<<0);
		}
		else if ( GETS2==1 && key2_up==1)
		{
            key2_up = 0;
			return iret &= ~(0x01<<1);
		}
		else if ( GETS3==1 && key3_up==1)
		{
            key3_up = 0;
			return iret &= ~(0x01<<2);
		}
		else if ( GETS4==1 && key4_up==1)
		{
            key4_up = 0;
			return iret &= ~(0x01<<3);
		}
		else if ( GETS5==1 && key5_up==1)
		{
            key5_up = 0;
			return iret &= ~(0x01<<4);
		}
		else if ( GETS6==1 && key6_up==1)
		{
            key6_up = 0;
			return iret &= ~(0x01<<5);
		}
		else if ( GETS7==1 && key7_up==1)
		{
            key7_up = 0;
			return iret &= ~(0x01<<6);
		}
		else if ( GETS8==1 && key8_up==1)
		{
            key8_up = 0;
			return iret &= ~(0x01<<7);
		}
		else if ( GETS9==1 && key9_up==1)
		{
            key9_up = 0;
		    return iret &= ~(0x01<<8);
		}    
		else if ( GETS10==1 && key10_up==1)
		{
            key10_up = 0;
		    return iret &= ~(0x01<<9);
		}
		else if ( GETS11==1 && key11_up==1)
		{
            key11_up = 0;
		    return iret &= ~(0x01<<10);
		}  
		else if ( GETS12==1 && key12_up==1)
		{
            key12_up = 0;
			return iret &= ~(0x01<<11);
		}
		else if ( GETS13==1 && key13_up==1)
		{
            key13_up = 0;
			return iret &= ~(0x01<<12);
		}  
		else if ( GETS14==1 && key14_up==1)
		{
            key14_up = 0;
			return iret &= ~(0x01<<13);
		}
		else if ( GETS15==1 && key15_up==1)
		{
            key15_up = 0;
			return iret &= ~(0x01<<14);
		}  
		else if ( GETS16==1 && key16_up==1)
		{
            key16_up = 0;
			return iret &= ~(0x01<<15);
		}
		else if ( GETS17==1 && key17_up==1)
		{
            key17_up = 0;
			return iret &= ~(0x01<<16);
		}  
		else if ( GETS18==1 && key18_up==1)
		{
            key18_up = 0;
			return iret &= ~(0x01<<17);
		}
		else if ( GETS19==1 && key19_up==1)
		{
            key19_up = 0;
			return iret &= ~(0x01<<18);
		}    
		else if ( GETS20==1 && key20_up==1)
		{
            key20_up = 0;
			return iret &= ~(0x01<<19);
		}
		else if ( GETS21==1 && key21_up==1)
		{
            key21_up = 0;
			return iret &= ~(0x01<<20);
		}  
		else if ( GETS22==1 && key22_up==1)
		{
            key22_up = 0;
			return iret &= ~(0x01<<21);
		}
		else if ( GETS23==1 && key23_up==1)
		{
            key23_up = 0;
			return iret &= ~(0x01<<22);
		}  
		else if ( GETS24==1 && key24_up==1)
		{
            key24_up = 0;
			return iret &= ~(0x01<<23);
		}        
	}

	return ERR_INFRARED;// 无按键按下
}





void bsp_GetSensorStatus(uint8_t *dat)
{
    static uint32_t code = 0;
    uint8_t bcd[3] = {0};
    uint8_t buf[6] = {0};
    
    if(GETS1 == 0)
    {
        code |= (0x01<<0);
    }
    else
    {
        code &= ~(0x01<<0);
    }

    if(GETS2 == 0)
    {
        code |= (0x01<<1);
    }
    else
    {
        code &= ~(0x01<<1);
    }
    
    if(GETS3 == 0)
    {
        code |= (0x01<<2);
    }
    else
    {
        code &= ~(0x01<<2);
    }
    

    if(GETS4 == 0)
    {
        code |= (0x01<<3);
    }
    else
    {
        code &= ~(0x01<<3);
    }
    

    if(GETS5 == 0)
    {
        code |= (0x01<<4);
    }
    else
    {
        code &= ~(0x01<<4);
    }

    if(GETS6 == 0)
    {
        code |= (0x01<<5);
    }
    else
    {
        code &= ~(0x01<<5);
    }
    
    if(GETS7 == 0)
    {
        code |= (0x01<<6);
    }
    else
    {
        code &= ~(0x01<<6);
    }
    

    if(GETS8 == 0)
    {
        code |= (0x01<<7);
    }    
    else
    {
        code &= ~(0x01<<7);
    }

////////////////////////////////////
    if(GETS9 == 0)
    {
        code |= (0x01<<8);
    }
    else
    {
        code &= ~(0x01<<8);
    }

    if(GETS10 == 0)
    {
        code |= (0x01<<9);
    }
    else
    {
        code &= ~(0x01<<9);
    }
    
    if(GETS11 == 0)
    {
        code |= (0x01<<10);
    }
    else
    {
        code &= ~(0x01<<10);
    }
    

    if(GETS12 == 0)
    {
        code |= (0x01<<11);
    }
    else
    {
        code &= ~(0x01<<11);
    }
    

    if(GETS13 == 0)
    {
        code |= (0x01<<12);
    }
    else
    {
        code &= ~(0x01<<12);
    }

    if(GETS14 == 0)
    {
        code |= (0x01<<13);
    }
    else
    {
        code &= ~(0x01<<13);
    }
    
    if(GETS15 == 0)
    {
        code |= (0x01<<14);
    }
    else
    {
        code &= ~(0x01<<14);
    }
    

    if(GETS16 == 0)
    {
        code |= (0x01<<15);
    }    
    else
    {
        code &= ~(0x01<<15);
    }
/////////////////////////////////////////////
    if(GETS17 == 0)
    {
        code |= (0x01<<16);
    }
    else
    {
        code &= ~(0x01<<16);
    }

    if(GETS18 == 0)
    {
        code |= (0x01<<17);
    }
    else
    {
        code &= ~(0x01<<17);
    }
    
    if(GETS19 == 0)
    {
        code |= (0x01<<18);
    }
    else
    {
        code &= ~(0x01<<18);
    }
    

    if(GETS20 == 0)
    {
        code |= (0x01<<19);
    }
    else
    {
        code &= ~(0x01<<19);
    }
    

    if(GETS21 == 0)
    {
        code |= (0x01<<20);
    }
    else
    {
        code &= ~(0x01<<20);
    }

    if(GETS22 == 0)
    {
        code |= (0x01<<21);
    }
    else
    {
        code &= ~(0x01<<21);
    }
    
    if(GETS23 == 0)
    {
        code |= (0x01<<22);
    }
    else
    {
        code &= ~(0x01<<22);
    }
    

    if(GETS24 == 0)
    {
        code |= (0x01<<23);
    }    
    else
    {
        code &= ~(0x01<<23);
    }
    

    bcd[0] = code>>16;    
    bcd[1] = code>>8;
    bcd[2] = code&0xff;
    
    memset(buf,0x00,sizeof(buf));

    bcd2asc(buf, bcd, 6, 0);
    
    memcpy(dat,buf,6);

}


//void bsp_putStatus(uint8_t dat)
//{
//    gSensorStatus[gSensorEnd] = dat;

//	if (++gSensorEnd  >= SENSORMAXBUFFSIZE)
//	{
//		gSensorEnd = 0;
//	}
//}


//uint32_t bsp_GetStatus(void)
//{
//	uint32_t ret;

//	if (gSensorTop == gSensorEnd)
//	{
//		return ERR_INFRARED;
//	}
//	else
//	{
//		ret = gSensorStatus[gSensorTop];

//		if (++gSensorTop >= SENSORMAXBUFFSIZE)
//		{
//			gSensorTop = 0;
//		}
//		return ret;
//	}
//}


