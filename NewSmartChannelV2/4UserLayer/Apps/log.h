#ifndef __LOG_H
#define __LOG_H


#include "stdio.h"

/** ��ĳһlog��������Ϊ6���ɵ����رոü����log��Ϣ */

#define     LOG_NONE            0
#define     LOG_ERROR           1
#define     LOG_WARNING         2
#define     LOG_INFO            3
#define     LOG_DEBUG           4
#define     LOG_VERBOSE         5
#define     LOG_ALL             6

/**
 * �����ڱ������� target options �ж�����Լ���
 * С�ڵ��� LOG_LEVEL �� LOG ������Ч
 */
#ifndef     LOG_LEVEL
    #define LOG_LEVEL           4
#endif

/**
 * �����º��log���߽�������
 * �ο�������Ӧ��ע��
 */
 
#define     logPrint            printf          /** ���ø�ʽ����ӡ���� */
#define     TIME_STAMP          0               /** ���û�ȡϵͳʱ��� */
#define     AUTO_TAG            0               /** Ϊ1ʱ�����Զ�����������ΪTAG��ӡ */
#define     LOG_END             "\r\n"          /** ��ӡ���з���β */


#if AUTO_TAG != 1

#if LOG_ERROR <= LOG_LEVEL
    #define logError(format, ...) \
            logPrint("[%d] [Error] "format""LOG_END, TIME_STAMP, ##__VA_ARGS__)
#else
    #define logError(format, ...)
#endif /** LOG_ERROR <= LOG_LEVEL */

#if LOG_WARNING <= LOG_LEVEL
    #define logWarning(format, ...) \
            logPrint("[%d] [Warning] "format""LOG_END, TIME_STAMP, ##__VA_ARGS__)
#else
    #define logWarning(format, ...)
#endif /** LOG_WARNING <= LOG_LEVEL */

#if LOG_INFO <= LOG_LEVEL
    #define logInfo(format, ...) \
            logPrint("[%d] [Info] "format""LOG_END, TIME_STAMP, ##__VA_ARGS__)
#else
    #define logInfo(format, ...)
#endif /** LOG_INFO <= LOG_LEVEL */

#if LOG_DEBUG <= LOG_LEVEL
    #define logDebugL(format, ...) \
            logPrint("[%d] [Debug] "format""LOG_END, TIME_STAMP, ##__VA_ARGS__)
    #define logDebug(format, ...) \
            logPrint("[%d] [Debug] [file:%s, line:%d] "format""LOG_END, \
                     TIME_STAMP, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define logDebug(format, ...)
    #define logDebugL(format, ...)
#endif /** LOG_DEBUG <= LOG_LEVEL */

#if LOG_VERBOSE <= LOG_LEVEL
    #define logVerbose(format, ...) \
            logPrint("[%d] [Verbose] "format""LOG_END, TIME_STAMP, ##__VA_ARGS__)
#else
    #define logVerbose(format, ...)
#endif /** LOG_VERBOSE <= LOG_LEVEL */

#else /** AUTO_TAG */

#if LOG_ERROR <= LOG_LEVEL
    #define logError(format, ...) \
            logPrint("[%d] [Error] %s: "format""LOG_END, \
                     TIME_STAMP, __FUNCTION__, ##__VA_ARGS__)
#else
    #define logError(format, ...)
#endif /** LOG_ERROR <= LOG_LEVEL */

#if LOG_WARNING <= LOG_LEVEL
    #define logWarning(format, ...) \
            logPrint("[%d] [Warning] %s: "format""LOG_END, \
                     TIME_STAMP, __FUNCTION__, ##__VA_ARGS__)
#else
    #define logWarning(format, ...)
#endif /** LOG_WARNING <= LOG_LEVEL */

#if LOG_INFO <= LOG_LEVEL
    #define logInfo(format, ...) \
            logPrint("[%d] [Info] %s: "format""LOG_END, \
                     TIME_STAMP, __FUNCTION__, ##__VA_ARGS__)
#else
    #define logInfo(format, ...)
#endif /** LOG_INFO <= LOG_LEVEL */

#if LOG_DEBUG <= LOG_LEVEL
    #define logDebug(format, ...) \
            logPrint("[%d] [Debug] %s: "format""LOG_END, \
                     TIME_STAMP, __FUNCTION__, ##__VA_ARGS__)
    #define logDebugL(format, ...) \
            logPrint("[%d] [Debug] [file:%s, line:%d] %s: "format""LOG_END, \
                     TIME_STAMP, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define logDebug(format, ...)
    #define logDebugL(format, ...)
#endif /** LOG_DEBUG <= LOG_LEVEL */

#if LOG_VERBOSE <= LOG_LEVEL
    #define logVerbose(format, ...) \
            logPrint("[%d] [Verbose] %s: "format""LOG_END, \
                     TIME_STAMP, __FUNCTION__, ##__VA_ARGS__)
#else
    #define logVerbose(format, ...)
#endif /** LOG_VERBOSE <= LOG_LEVEL */

#endif /** AUTO_TAG */




#endif

