/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : ver.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年8月12日
  最近修改   :
  功能描述   : 实现版本管理
  函数列表   :
  修改历史   :
  1.日    期   : 2019年8月12日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#include "ver.h"


const  unsigned char g_revision_date[12] = __DATE__;
const  unsigned char g_revision_time[16] = __TIME__;


const unsigned char VersionString[] = "1.0.2.1";
const unsigned char VersionDate[] = __DATE__ " " __TIME__;


const VERSION PosVersion = {VersionString, VersionDate};


