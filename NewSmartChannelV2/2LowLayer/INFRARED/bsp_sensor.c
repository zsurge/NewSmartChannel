/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : bsp_sensor.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年5月22日
  最近修改   :
  功能描述   : 红外对射驱动
  函数列表   :
  修改历史   :
  1.日    期   : 2020年5月22日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "bsp_sensor.h"
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
SENSORVALUE_STRU gSensorValue;

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
void bsp_sensor_init(void)
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

	memset ( &gSensorValue,0,sizeof ( SENSORVALUE_STRU ) );

}


#if 0
uint32_t bsp_sensor_scan(void)
{
    uint8_t i = 0;
    
	gSensorValue.state[0]=GETS1;
	gSensorValue.state[1]=GETS2;
	gSensorValue.state[2]=GETS3;
	gSensorValue.state[3]=GETS4;
	gSensorValue.state[4]=GETS5;
	gSensorValue.state[5]=GETS6;
	gSensorValue.state[6]=GETS7;
	gSensorValue.state[7]=GETS8;
	gSensorValue.state[8]=GETS9;
	gSensorValue.state[9]=GETS10;
	gSensorValue.state[10]=GETS11;
	gSensorValue.state[11]=GETS12;
	gSensorValue.state[12]=GETS13;
	gSensorValue.state[13]=GETS14;
	gSensorValue.state[14]=GETS15;
	gSensorValue.state[15]=GETS16;
	gSensorValue.state[16]=GETS17;
	gSensorValue.state[17]=GETS18;
	gSensorValue.state[18]=GETS19;
	gSensorValue.state[19]=GETS20;
	gSensorValue.state[20]=GETS21;
	gSensorValue.state[21]=GETS22;
	gSensorValue.state[22]=GETS23;
	gSensorValue.state[23]=GETS24;


	for(i=0; i<SENSOR_NUM_MAX; i++)
	{
		if (gSensorValue.state[i] == TURN_ON)  //红外触发
		{
            gSensorValue.code |= ( 0x01<<i );		
		}
		else //离开
		{
			gSensorValue.code &= ~ ( 0x01<<i );
		}
	}

	if ( gSensorValue.code != gSensorValue.preCode )
	{
		gSensorValue.preCode = gSensorValue.code;
		return gSensorValue.code;
	}	

	

   	return ERR_INFRARED;// 无按键按下
}

#else

uint32_t bsp_sensor_scan(void)
{
    uint8_t i = 0;

    //给上一次红外赋初值
    memcpy(gSensorValue.state,gSensorValue.curSensorVal,SENSOR_NUM_MAX);
    
	gSensorValue.curSensorVal[0]=GETS1;
	gSensorValue.curSensorVal[1]=GETS2;
	gSensorValue.curSensorVal[2]=GETS3;
	gSensorValue.curSensorVal[3]=GETS4;
	gSensorValue.curSensorVal[4]=GETS5;
	gSensorValue.curSensorVal[5]=GETS6;
	gSensorValue.curSensorVal[6]=GETS7;
	gSensorValue.curSensorVal[7]=GETS8;
	gSensorValue.curSensorVal[8]=GETS9;
	gSensorValue.curSensorVal[9]=GETS10;
	gSensorValue.curSensorVal[10]=GETS11;
	gSensorValue.curSensorVal[11]=GETS12;
	gSensorValue.curSensorVal[12]=GETS13;
	gSensorValue.curSensorVal[13]=GETS14;
	gSensorValue.curSensorVal[14]=GETS15;
	gSensorValue.curSensorVal[15]=GETS16;
	gSensorValue.curSensorVal[16]=GETS17;
	gSensorValue.curSensorVal[17]=GETS18;
	gSensorValue.curSensorVal[18]=GETS19;
	gSensorValue.curSensorVal[19]=GETS20;
	gSensorValue.curSensorVal[20]=GETS21;
	gSensorValue.curSensorVal[21]=GETS22;
	gSensorValue.curSensorVal[22]=GETS23;
	gSensorValue.curSensorVal[23]=GETS24;

    
	for(i=0; i<SENSOR_NUM_MAX; i++)
	{
        if(gSensorValue.curSensorVal[i] == gSensorValue.state[i])
        {	
    		if (gSensorValue.state[i] == TURN_ON)  //红外触发
    		{
                gSensorValue.code |= ( 0x01<<i );
    		}
    		else //离开
    		{
    			gSensorValue.code &= ~ ( 0x01<<i );
    		}
		}
		else
		{
		    gSensorValue.code &= ~ ( 0x01<<i );
		}
	}

	if ( gSensorValue.code != gSensorValue.preCode )
	{
		gSensorValue.preCode = gSensorValue.code;
		
		return gSensorValue.code;
	}

   	return ERR_INFRARED;// 无按键按下
}



#endif

void bsp_GetSensorValue(uint8_t *dat)
{
	//static uint32_t code = 0;
	uint8_t bcd[3+1] = {0};
	uint8_t buf[6+1] = {0};
	//uint8_t i;

    memset(bcd,0x00,sizeof(bcd));
    memset ( buf,0x00,sizeof ( buf ) );
    
	bcd[0] = gSensorValue.code>>16;
	bcd[1] = gSensorValue.code>>8;
	bcd[2] = gSensorValue.code&0xff;	
	
	bcd2asc ( buf, bcd, 6, 0 );
	memcpy ( dat,buf,6 );

    printf("bsp_GetSensorStatus = %s\r\n",buf);
}



