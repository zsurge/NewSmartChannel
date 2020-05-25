 /*
********************************************************************************
*
*                                 Queue.h
*
* File          : Queue.h
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : 队列操作头文件
*                
* History       :
* Date          : 2013.07.22
*******************************************************************************/


#ifndef _UARTQUEUE_H_
#define _UARTQUEUE_H_

#ifdef  __cplusplus
extern "C"
{
#endif



#include "stdint.h"

#define DEBUG_FULL_ASSERT       0

#ifdef DEBUG_FULL_ASSERT
#define ASSERT_PARAM(a)          ((a) ? (void)0 : ASSERT_FAILED((uint8_t *)__FILE__, __LINE__))
void ASSERT_FAILED(uint8_t* file, uint32_t line);
#else
#define ASSERT_PARAM(a)           if (a == NULL) return 0;
#endif





typedef struct {
    volatile uint32_t     bufSize;
    volatile uint8_t      *pStart;
    volatile uint8_t      *pEnd;
    volatile uint8_t      *pBuf;
}QUEUE8_TYPE;




uint32_t QUEUE_PacketCreate(QUEUE8_TYPE *pQ8, uint8_t *pBuf, uint32_t bufSize);
uint32_t QUEUE_PacketIn(QUEUE8_TYPE *pQ8, uint8_t *pData, uint32_t len);
uint32_t QUEUE_PacketOut(QUEUE8_TYPE *pQ8, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketSplit(QUEUE8_TYPE *pQ8, uint8_t startChar, uint8_t endChar, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketDoubleByteSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketCharSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen);
uint32_t QUEUE_PacketDoubleCharSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar1, uint8_t splitChar2, uint8_t *pData, uint32_t dataLen);


#ifdef  __cplusplus
}
#endif

#endif

