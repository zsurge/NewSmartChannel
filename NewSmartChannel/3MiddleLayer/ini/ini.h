/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : ini.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月28日
  最近修改   :
  功能描述   : 对参数进行操作
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月28日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

#ifndef __INI_H
#define __INI_H
#include "easyflash.h"
#include "sfud.h"
#include "errorcode.h"
#include "tool.h"

void ReadIAP(void);

void RestoreDefaultSetting(void);

void SystemReset(void);

SYSERRORCODE_E RecordBootTimes(void);







#endif
