/*
 ********************************************************************************
 *
 *                                 UARTQueue.c
 *
 * File          : UARTQueue.c
 * Version       : V1.0
 * Author        : whq
 * Mode          : Thumb2
 * Toolchain     : 
 * Description   : 队列操作
 *                
 * History       :
 * Date          : 2013.07.22
 *******************************************************************************/
 
 #include <string.h>
 
 #include "UARTQueue.h"
 /*******************************************************************************
 * Function Name : uint32_t QUEUE_PacketCreate(QUEUE8_TYPE *pQ8, uint8_t *pBuf, uint32_t lenSize)
 * Description   : 队列建立
 * Input         :   pQ8:    队列
                     pBuf:   队列缓冲区地址
                     bufSize:队列缓冲区大小
 * Output        : 
 * Other         : 
 * Date          : 2013.08.29
 *******************************************************************************/
 uint32_t QUEUE_PacketCreate(QUEUE8_TYPE *pQ8, uint8_t *pBuf, uint32_t bufSize)
 {
     ASSERT_PARAM(pQ8);
     ASSERT_PARAM(pBuf);
     ASSERT_PARAM(bufSize);
 
     pQ8->bufSize    = bufSize;
     pQ8->pBuf       = pBuf;
     pQ8->pStart     = pBuf;
     pQ8->pEnd       = pBuf;
 
     return 0;
 }
 
 /*******************************************************************************
 * Function Name : uint32_t QUEUE_PacketIn(QUEUE8_TYPE *pQ8, uint8_t *pData, uint32_t len)
 * Description   : 数据载入队列
 * Input         :   pQ8:    队列
                     pData:  要进队列的数据
                     len:    数据长度
 * Output        : 
 * Other         : 
 * Date          : 2013.08.29
 *******************************************************************************/
 uint32_t QUEUE_PacketIn(QUEUE8_TYPE *pQ8, uint8_t *pData, uint32_t len)
 {
     uint32_t dataLen = len;
     
     ASSERT_PARAM(pData);
     ASSERT_PARAM(pQ8);
     ASSERT_PARAM(pQ8->pStart);
     ASSERT_PARAM(pQ8->pEnd);
 
     while (dataLen--)
     {
         *pQ8->pEnd++ = *pData++;
         
         if (pQ8->pEnd >= pQ8->pBuf + pQ8->bufSize)      //指针指向栈尾
         {
             pQ8->pEnd = pQ8->pBuf;
         }
         
         if (pQ8->pEnd == pQ8->pStart)                   //缓冲区填满 覆盖最早的数据
         {
             pQ8->pStart++;
             if (pQ8->pStart >= pQ8->pBuf + pQ8->bufSize) 
             {
                 pQ8->pStart = pQ8->pBuf;
             }
         }
     }
     
     return len;
 }
 
 /*******************************************************************************
 * Function Name : uint32_t QUEUE_PacketOut(QUEUE8_TYPE *pQ8, uint8_t *pData, uint32_t dataLen)
 * Description   : 队列中取数据
 * Input         :   pQ8:    队列
                     pData:  缓冲区
                     dataLen:缓冲区大小
 * Output        : 
 * Other         : 
 * Date          : 2013.08.29
 *******************************************************************************/
 uint32_t QUEUE_PacketOut(QUEUE8_TYPE *pQ8, uint8_t *pData, uint32_t dataLen)
 {
     uint32_t index = 0;
     
     ASSERT_PARAM(pData);
     ASSERT_PARAM(pQ8);
     ASSERT_PARAM(pQ8->pStart);
     ASSERT_PARAM(pQ8->pEnd);
 
     while ((pQ8->pStart != pQ8->pEnd) && (index < dataLen) && (index < pQ8->bufSize))
     {
         pData[index++] = *pQ8->pStart++;
         if (pQ8->pStart >= pQ8->pBuf + pQ8->bufSize) 
         {
             pQ8->pStart = pQ8->pBuf;
         }
     }
 
     return index;
 }
 
 /*******************************************************************************
 * Function Name : uint32_t QUEUE_PacketSplit(QUEUE8_TYPE *pQ8, uint8_t startChar, uint8_t endChar, uint8_t *pData, uint32_t dataLen)
 * Description   : 以起始符和结束符取队列中的数据 (取出的数据 包括起始符 和分隔符)
 * Input         :   pQ8:        队列
                     startChar:  起始符
                     endChar:    结束符
                     pData:      缓冲区
                     dataLen:    缓冲区大小
 * Output        : 
 * Other         : 
 * Date          : 2013.08.29
 *******************************************************************************/
 uint32_t QUEUE_PacketSplit(QUEUE8_TYPE *pQ8, uint8_t startChar, uint8_t endChar, uint8_t *pData, uint32_t dataLen)
 {
     int32_t count;
     int32_t index;
     volatile uint8_t *pStart;
     volatile uint8_t *pEnd;
     
     ASSERT_PARAM(pData);
     ASSERT_PARAM(pQ8);
     ASSERT_PARAM(pQ8->pStart);
     ASSERT_PARAM(pQ8->pEnd);
 
     pStart      = pQ8->pStart;
     count       = pQ8->bufSize;
     
     while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
     {
         if (startChar == *pStart) break;
         if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
 
     if (pStart == pQ8->pEnd) return 0;              //未找到起始符
     if (count == -1) return 0;
     pEnd = pStart;
     if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
 
     while ((pEnd != pQ8->pEnd) && count--)          //查找结束字符
     {
         if (endChar == *pEnd) break;
         if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
     }
     
     if (pEnd == pQ8->pEnd) return 0;                //未找结束符
     if (count == -1) return 0;
     if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
     
     count   = pQ8->bufSize - count;   
     index   = 0;
      //获取从起始字符到结束字符的数据
     while ((pStart != pEnd) && (index < dataLen) && (index < pQ8->bufSize) && count--)
     {
         pData[index++] = *pStart++;
         if (pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
 
     pQ8->pStart = pEnd;
     return index;
 }
 
 /*******************************************************************************
 * Function Name : uint32_t QUEUE_PacketCharSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
 * Description   : 提取首尾双分隔符内的数据(包括分隔符)
 * Input         :   pQ8:        队列
                     startChar:  起始符
                     endChar:    结束符
                     pData:      缓冲区
                     dataLen:    缓冲区大小
 * Output        : 
 * Other         : 
 * Date          : 2013.08.30
 *******************************************************************************/
 uint32_t QUEUE_PacketDoubleByteSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
 {
     int32_t count;
     int32_t index;
     volatile uint8_t *pStart;
     volatile uint8_t *pEnd;
     
     ASSERT_PARAM(pData);
     ASSERT_PARAM(pQ8);
     ASSERT_PARAM(pQ8->pStart);
     ASSERT_PARAM(pQ8->pEnd);
 
     pStart      = pQ8->pStart;
     count       = pQ8->bufSize;
     
     while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
     {
         if (splitChar == *pStart) break;
         if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
 
     if (pStart == pQ8->pEnd) return 0;              //未找到起始符
     if (count == -1) return 0;
     pEnd = pStart;
     if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
 
     while ((pEnd != pQ8->pEnd) && count--)          //查找结束字符
     {
         if (splitChar == *pEnd) break;
         if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
     }
     
     if (pEnd == pQ8->pEnd) return 0;                //未找结束符
     if (count == -1) return 0;
     if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
     
     count   = pQ8->bufSize - count;   
     index   = 0;
      //获取从起始字符到结束字符的数据
     while ((pStart != pEnd) && (index < dataLen) && (index < pQ8->bufSize) && count--)
     {
         pData[index++] = *pStart++;
         if (pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
 
     //如果取出的数据只包括分隔符，有可能是上次结束符和下次起始符，因此放弃上次结束符。
     if (index <= 2)                                  
     {
         index = 0;
         if (--pStart < pQ8->pBuf) pStart = pQ8->pBuf + pQ8->bufSize - 1;
     }
 
     pQ8->pStart = pStart;
     return index;
 }
 
 /*******************************************************************************
 * Function Name : uint32_t QUEUE_PacketCharSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
 * Description   : 提取单结束分隔符的数据 (包括分隔符)
 * Input         : 
 * Output        : 
 * Other         : 
 * Date          : 2013.10.20
 *******************************************************************************/
 uint32_t QUEUE_PacketCharSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar, uint8_t *pData, uint32_t dataLen)
 {
     int32_t count;
     int32_t index;
     volatile uint8_t *pStart;
     volatile uint8_t *pEnd;
     
     ASSERT_PARAM(pData);
     ASSERT_PARAM(pQ8);
     ASSERT_PARAM(pQ8->pStart);
     ASSERT_PARAM(pQ8->pEnd);
 
     pStart      = pQ8->pStart;
     count       = pQ8->bufSize;
 
     while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
     {
         if (splitChar == *pStart) break;
         if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
 
     if (pStart == pQ8->pEnd) return 0;              //未找到起始符
     if (count == -1) return 0;
     pEnd = pStart;
     if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
     
     pStart      = pQ8->pStart;
     count       = pQ8->bufSize;
     index       = 0;
     while ((pStart != pEnd) && (index < dataLen) && count--)        //查找起始字符
     {
         pData[index++] = *pStart;
         if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
     
     pQ8->pStart = pStart;
     return index;
 }
 
 /*******************************************************************************
 * Function Name :QUEUE_PacketDoubleCharSplit
 * Description   :提取双结束分隔符的数据 (包括分隔符)
 * Input         :QUEUE8_TYPE * pQ8
 * Input         :uint8_t splitChar1
 * Input         :uint8_t splitChar2
 * Input         :uint8_t * pData
 * Input         :uint32_t dataLen
 * Output        :uint32_t
 * Other         :
 * Date          :2014/03/27
 *******************************************************************************/
 uint32_t QUEUE_PacketDoubleCharSplit(QUEUE8_TYPE *pQ8, uint8_t splitChar1, uint8_t splitChar2, uint8_t *pData, uint32_t dataLen)
 {
     int32_t count;
     int32_t index;
     volatile uint8_t *pStart;
     volatile uint8_t *pEnd;
     uint8_t lastChar = 0;
 
     ASSERT_PARAM(pData);
     ASSERT_PARAM(pQ8);
     ASSERT_PARAM(pQ8->pStart);
     ASSERT_PARAM(pQ8->pEnd);
 
     pStart      = pQ8->pStart;
     count       = pQ8->bufSize;
 
     while ((pStart != pQ8->pEnd) && count--)        //查找起始字符
     {
         if ((splitChar1 == lastChar) && (splitChar2 == *pStart)) break;
 
         lastChar = *pStart;
 
         if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
 
     if (pStart == pQ8->pEnd) return 0;              //未找到起始符
     if (count == -1) return 0;
     pEnd = pStart;
     if (++pEnd >= pQ8->pBuf + pQ8->bufSize) pEnd = pQ8->pBuf;
 
     pStart      = pQ8->pStart;
     count       = pQ8->bufSize;
     index       = 0;
     while ((pStart != pEnd) && (index < dataLen) && count--)        //查找起始字符
     {
         pData[index++] = *pStart;
         if (++pStart >= pQ8->pBuf + pQ8->bufSize) pStart = pQ8->pBuf;
     }
 
     pQ8->pStart = pStart;
     return index;
 }
 
 
 /*******************************************************************************
 * Function Name : void ASSERT_FAILED(uint8_t* file, uint32_t line)
 * Description   : 异常
 * Input         : 
 * Output        : 
 * Other         : 
 * Date          : 2013.08.29
 *******************************************************************************/
 void ASSERT_FAILED(uint8_t* file, uint32_t line)
 {
     uint8_t flg = 1;
 
     while (flg);
 }

