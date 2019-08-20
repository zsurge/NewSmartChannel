/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : ver.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年8月12日
  最近修改   :
  功能描述   : 进行程序版本管理
  函数列表   :
  修改历史   :
  1.日    期   : 2019年8月12日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

#ifndef __VER_H
#define __VER_H
#include "stm32f4xx_conf.h"


extern const uint8_t g_revision_date[12];
extern const uint8_t g_revision_time[16];

typedef struct
{
	const unsigned char *vString;
	const unsigned char *vDate;
}VERSION_T;

extern const VERSION_T DevVersion;



#endif
----------
