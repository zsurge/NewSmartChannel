#include "bsp_infrared.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "tool.h"

uint32_t readData = 0;

//下降沿触发
uint32_t keyDown = 0;
//按键持续按下
uint32_t keyContinue = 0;
//上升沿触发
uint32_t keyUp = 0;

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

	GPIO_SetBits ( GPIO_PORT_SENSOR14,GPIO_PIN_SENSOR1|GPIO_PIN_SENSOR2|GPIO_PIN_SENSOR3|GPIO_PIN_SENSOR4 );
	GPIO_SetBits ( GPIO_PORT_SENSOR56,GPIO_PIN_SENSOR5 |GPIO_PIN_SENSOR6 );
	GPIO_SetBits ( GPIO_PORT_SENSOR78,GPIO_PIN_SENSOR7 |GPIO_PIN_SENSOR8 );


}




//读取按键端口状态返回按键ID
static uint32_t bsp_getKeyState ( void )
{
    static uint32_t code = 0;   

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

    return code;

}

uint32_t bsp_infrared_scan ( void )
{	
    static uint32_t iret = 0;

	//程序核心部分，以下四条语句实现按键扫描
	readData = bsp_getKeyState();

	//记录下降沿触发 ，仅出现一次，出现后自消除，持续时间 = 扫描时间，相当于一个脉冲
	keyDown  = readData & ( readData ^ keyContinue );
    
	//记录上升沿触发 ，仅出现一次，出现后自消除，持续时间 = 扫描时间 ，相当于一个脉冲
	keyUp =  readData ^ ( readData | keyContinue );	
    
	//按键持续按下记录
	keyContinue = readData;

//扫描后处理，根据实际情况自定义
	//按键按下处理
	if ( keyDown )
	{
		switch ( keyDown )
		{
			case KEY1_ID:
                return iret |= (0x01<<0);
			case KEY2_ID:
				return iret |= (0x01<<1);
			case KEY3_ID:
				return iret |= (0x01<<2);
			case KEY4_ID:
				return iret |= (0x01<<3);
			case KEY5_ID:
				return iret |= (0x01<<4);
			case KEY6_ID:
				return iret |= (0x01<<5);
			case KEY7_ID:
				return iret |= (0x01<<6);
			case KEY8_ID:
				return iret |= (0x01<<7);
			case KEY9_ID:
				return iret |= (0x01<<8);
			case KEY10_ID:
				return iret |= (0x01<<9);
			case KEY11_ID:
				return iret |= (0x01<<10);
			case KEY12_ID:
				return iret |= (0x01<<11);
			case KEY13_ID:
				return iret |= (0x01<<12);
			case KEY14_ID:
				return iret |= (0x01<<13);
			case KEY15_ID:
				return iret |= (0x01<<14);
			case KEY16_ID:
				return iret |= (0x01<<15);
			case KEY17_ID:
				return iret |= (0x01<<16);
			case KEY18_ID:
				return iret |= (0x01<<17);
			case KEY19_ID:
				return iret |= (0x01<<18);
			case KEY20_ID:
				return iret |= (0x01<<19);
			case KEY21_ID:
				return iret |= (0x01<<20);
			case KEY22_ID:
				return iret |= (0x01<<21);
			case KEY23_ID:
				return iret |= (0x01<<22);
			case KEY24_ID:
				return iret |= (0x01<<23);
            default: 
                break;
		}
	}
	//按键接起后处理
	else if ( keyUp )
	{
		switch ( keyUp )
		{            
			case KEY1_ID:
				return iret &= ~(0x01<<0);
			case KEY2_ID:
				return iret &= ~(0x01<<1);
			case KEY3_ID:
				return iret &= ~(0x01<<2);
			case KEY4_ID:
				return iret &= ~(0x01<<3);
			case KEY5_ID:
				return iret &= ~(0x01<<4);
			case KEY6_ID:
				return iret &= ~(0x01<<5);
			case KEY7_ID:
				return iret &= ~(0x01<<6);
			case KEY8_ID:
				return iret &= ~(0x01<<7);
			case KEY9_ID:
				return iret &= ~(0x01<<8);
			case KEY10_ID:
				return iret &= ~(0x01<<9);
			case KEY11_ID:
				return iret &= ~(0x01<<10);
			case KEY12_ID:
				return iret &= ~(0x01<<11);
			case KEY13_ID:
				return iret &= ~(0x01<<12);
			case KEY14_ID:
				return iret &= ~(0x01<<13);
			case KEY15_ID:
				return iret &= ~(0x01<<14);
			case KEY16_ID:
				return iret &= ~(0x01<<15);
			case KEY17_ID:
				return iret &= ~(0x01<<16);
			case KEY18_ID:
				return iret &= ~(0x01<<17);
			case KEY19_ID:
				return iret &= ~(0x01<<18);
			case KEY20_ID:
				return iret &= ~(0x01<<19);
			case KEY21_ID:
				return iret &= ~(0x01<<20);
			case KEY22_ID:
				return iret &= ~(0x01<<21);
			case KEY23_ID:
				return iret &= ~(0x01<<22);
			case KEY24_ID:
				return iret &= ~(0x01<<23);
            default:
                break;                
		}

	}

    return ERR_INFRARED;// 无按键按下
    
}


void bsp_GetSensorStatus ( uint8_t* dat )
{

    static uint32_t code = 0;
    uint8_t bcd[3] = {0};
    uint8_t buf[6] = {0};
    

    code = bsp_getKeyState();

    bcd[0] = code>>16;    
    bcd[1] = code>>8;
    bcd[2] = code&0xff;
    
    memset(buf,0x00,sizeof(buf));

    bcd2asc(buf, bcd, 6, 0);
    
    memcpy(dat,buf,6);
}


