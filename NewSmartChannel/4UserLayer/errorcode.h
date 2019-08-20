/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : errorcode.h
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年8月20日
  最近修改   :
  功能描述   : 系统错误代码
  函数列表   :
  修改历史   :
  1.日    期   : 2019年8月20日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#ifndef __ERRORCODE_H_
#define __ERRORCODE_H_

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/   
#include "stm32f4xx_conf.h"


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
typedef enum {
    NO_ERR = 0,
    //系统异常类错误代码
    CJSON_PACKET_ERR,   //cjson 打包失败
    CJSON_PARSE_ERR,    //cjson 解析失败
    CJSON_GETITEM_ERR,  //cjson 获取员素失败
    COMM_PARSE_ERR,     //指令解析失败
    COMM_CRC_ERR,      //解析指令，CRC校验失败
    FLASH_W_ERR,        //写FLASH失败
    //韦根读卡错误代码
    READ_ERR = 1000,
    //QR扫码器错误代码
    //A门电机错误代码
    //B门电机错误代码    

} SYSERRORCODE_E;

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/



#endif


