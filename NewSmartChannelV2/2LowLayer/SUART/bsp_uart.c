/*
********************************************************************************
*
*                                 BSP_Uart.c
*
* File          : BSP_Uart.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : 串口驱动程序
*                
* History       :
* Date          : 2013.08.12
*******************************************************************************/
#include <string.h>

#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_dma.h"


#include "UARTQueue.h"
#include "BSP_Uart.h"


#if COM1_EN
static uint8_t COM1_TX_BUFF[BSP_COM1_Tx_BUFF_SIZE] = {0};
static uint8_t COM1_RX_BUFF[BSP_COM1_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM1_RX_QUEUE = {0};
#endif

#if COM2_EN
static uint8_t COM2_TX_BUFF[BSP_COM2_Tx_BUFF_SIZE] = {0};
static uint8_t COM2_RX_BUFF[BSP_COM2_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM2_RX_QUEUE = {0};
#endif

#if COM3_EN
static uint8_t COM3_TX_BUFF[BSP_COM3_Tx_BUFF_SIZE] = {0};
static uint8_t COM3_RX_BUFF[BSP_COM3_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM3_RX_QUEUE = {0};
#endif

#if COM4_EN
static uint8_t COM4_TX_BUFF[BSP_COM4_Tx_BUFF_SIZE] = {0};
static uint8_t COM4_RX_BUFF[BSP_COM4_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM4_RX_QUEUE = {0};
#endif

#if COM5_EN
static uint8_t COM5_TX_BUFF[BSP_COM5_Tx_BUFF_SIZE] = {0};
static uint8_t COM5_RX_BUFF[BSP_COM5_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM5_RX_QUEUE = {0};
#endif

#if COM6_EN
static uint8_t COM6_TX_BUFF[BSP_COM6_Tx_BUFF_SIZE] = {0};
static uint8_t COM6_RX_BUFF[BSP_COM6_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM6_RX_QUEUE = {0};
#endif

#if COM7_EN
static uint8_t COM7_TX_BUFF[BSP_COM7_Tx_BUFF_SIZE] = {0};
static uint8_t COM7_RX_BUFF[BSP_COM7_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM7_RX_QUEUE = {0};
#endif

#if COM8_EN
static uint8_t COM8_TX_BUFF[BSP_COM8_Tx_BUFF_SIZE] = {0};
static uint8_t COM8_RX_BUFF[BSP_COM8_Rx_BUFF_SIZE] = {0};
static QUEUE8_TYPE COM8_RX_QUEUE = {0};
#endif



static USART_TypeDef* const COM_USART[COMn] = {  
#if     COM1_EN
    BSP_COM1,
#endif
#if     COM2_EN
    BSP_COM2,
#endif
#if     COM3_EN
    BSP_COM3,
#endif
#if     COM4_EN
    BSP_COM4,
#endif
#if     COM5_EN
    BSP_COM5,
#endif
#if     COM6_EN
    BSP_COM6,
#endif
#if     COM7_EN
    BSP_COM7,
#endif
#if     COM8_EN
    BSP_COM8,
#endif
}; 

static const uint8_t COM_AF[COMn] = {
#if     COM1_EN
    BSP_COM1_AF,
#endif
#if     COM2_EN
    BSP_COM2_AF,
#endif
#if     COM3_EN
    BSP_COM3_AF,
#endif
#if     COM4_EN
    BSP_COM4_AF,
#endif
#if     COM5_EN
    BSP_COM5_AF,
#endif
#if     COM6_EN
    BSP_COM6_AF,
#endif
#if     COM7_EN
    BSP_COM7_AF,
#endif
#if     COM8_EN
    BSP_COM8_AF,
#endif
};
                                                
static const uint8_t COM_TX_AF_PIN[COMn] = {   
#if     COM1_EN
    BSP_COM1_TX_AF_PIN,
#endif
#if     COM2_EN
    BSP_COM2_TX_AF_PIN,
#endif
#if     COM3_EN
    BSP_COM3_TX_AF_PIN,
#endif
#if     COM4_EN
    BSP_COM4_TX_AF_PIN,
#endif
#if     COM5_EN
    BSP_COM5_TX_AF_PIN,
#endif
#if     COM6_EN
    BSP_COM6_TX_AF_PIN,
#endif
#if     COM7_EN
    BSP_COM7_TX_AF_PIN,
#endif
#if     COM8_EN
    BSP_COM8_TX_AF_PIN,
#endif
};

static const uint8_t COM_RX_AF_PIN[COMn] = {   
#if     COM1_EN
    BSP_COM1_RX_AF_PIN,
#endif
#if     COM2_EN
    BSP_COM2_RX_AF_PIN,
#endif
#if     COM3_EN
    BSP_COM3_RX_AF_PIN,
#endif
#if     COM4_EN
    BSP_COM4_RX_AF_PIN,
#endif
#if     COM5_EN
    BSP_COM5_RX_AF_PIN,
#endif
#if     COM6_EN
    BSP_COM6_RX_AF_PIN,
#endif
#if     COM7_EN
    BSP_COM7_RX_AF_PIN,
#endif
#if     COM8_EN
    BSP_COM8_RX_AF_PIN,
#endif
};

static GPIO_TypeDef* const COM_TX_PORT[COMn] = {  
#if     COM1_EN
    BSP_COM1_TX_GPIO_PORT,
#endif
#if     COM2_EN
    BSP_COM2_TX_GPIO_PORT,
#endif
#if     COM3_EN
    BSP_COM3_TX_GPIO_PORT,
#endif
#if     COM4_EN
    BSP_COM4_TX_GPIO_PORT,
#endif
#if     COM5_EN
    BSP_COM5_TX_GPIO_PORT,
#endif
#if     COM6_EN
    BSP_COM6_TX_GPIO_PORT,
#endif
#if     COM7_EN
    BSP_COM7_TX_GPIO_PORT,
#endif
#if     COM8_EN
    BSP_COM8_TX_GPIO_PORT,
#endif
};
 
static GPIO_TypeDef* const COM_RX_PORT[COMn] = {     
#if     COM1_EN
    BSP_COM1_RX_GPIO_PORT,
#endif
#if     COM2_EN
    BSP_COM2_RX_GPIO_PORT,
#endif
#if     COM3_EN
    BSP_COM3_RX_GPIO_PORT,
#endif
#if     COM4_EN
    BSP_COM4_RX_GPIO_PORT,
#endif
#if     COM5_EN
    BSP_COM5_RX_GPIO_PORT,
#endif
#if     COM6_EN
    BSP_COM6_RX_GPIO_PORT,
#endif
#if     COM7_EN
    BSP_COM7_RX_GPIO_PORT,
#endif
#if     COM8_EN
    BSP_COM8_RX_GPIO_PORT,
#endif
};
 
static const uint32_t COM_USART_CLK[COMn] = {   
#if     COM1_EN
    BSP_COM1_CLK,
#endif
#if     COM2_EN
    BSP_COM2_CLK,
#endif
#if     COM3_EN
    BSP_COM3_CLK,
#endif
#if     COM4_EN
    BSP_COM4_CLK,
#endif
#if     COM5_EN
    BSP_COM5_CLK,
#endif
#if     COM6_EN
    BSP_COM6_CLK,
#endif
#if     COM7_EN
    BSP_COM7_CLK,
#endif
#if     COM8_EN
    BSP_COM8_CLK,
#endif
};
                
static const uint32_t COM_TX_PORT_CLK[COMn] = { 
#if     COM1_EN
    BSP_COM1_TX_GPIO_CLK,
#endif
#if     COM2_EN
    BSP_COM2_TX_GPIO_CLK,
#endif
#if     COM3_EN
    BSP_COM3_TX_GPIO_CLK,
#endif
#if     COM4_EN
    BSP_COM4_TX_GPIO_CLK,
#endif
#if     COM5_EN
    BSP_COM5_TX_GPIO_CLK,
#endif
#if     COM6_EN
    BSP_COM6_TX_GPIO_CLK,
#endif
#if     COM7_EN
    BSP_COM7_TX_GPIO_CLK,
#endif
#if     COM8_EN
    BSP_COM8_TX_GPIO_CLK,
#endif
};
 
static const uint32_t COM_RX_PORT_CLK[COMn] = {
#if     COM1_EN
    BSP_COM1_RX_GPIO_CLK,
#endif
#if     COM2_EN
    BSP_COM2_RX_GPIO_CLK,
#endif
#if     COM3_EN
    BSP_COM3_RX_GPIO_CLK,
#endif
#if     COM4_EN
    BSP_COM4_RX_GPIO_CLK,
#endif
#if     COM5_EN
    BSP_COM5_RX_GPIO_CLK,
#endif
#if     COM6_EN
    BSP_COM6_RX_GPIO_CLK,
#endif
#if     COM7_EN
    BSP_COM7_RX_GPIO_CLK,
#endif
#if     COM8_EN
    BSP_COM8_RX_GPIO_CLK,
#endif
};

static const uint16_t COM_TX_PIN[COMn] = {      
#if     COM1_EN
    BSP_COM1_TX_PIN,
#endif
#if     COM2_EN
    BSP_COM2_TX_PIN,
#endif
#if     COM3_EN
    BSP_COM3_TX_PIN,
#endif
#if     COM4_EN
    BSP_COM4_TX_PIN,
#endif
#if     COM5_EN
    BSP_COM5_TX_PIN,
#endif
#if     COM6_EN
    BSP_COM6_TX_PIN,
#endif
#if     COM7_EN
    BSP_COM7_TX_PIN,
#endif
#if     COM8_EN
    BSP_COM8_TX_PIN,
#endif
};

static const uint16_t COM_RX_PIN[COMn] = { 
#if     COM1_EN
    BSP_COM1_RX_PIN,
#endif
#if     COM2_EN
    BSP_COM2_RX_PIN,
#endif
#if     COM3_EN
    BSP_COM3_RX_PIN,
#endif
#if     COM4_EN
    BSP_COM4_RX_PIN,
#endif
#if     COM5_EN
    BSP_COM5_RX_PIN,
#endif
#if     COM6_EN
    BSP_COM6_RX_PIN,
#endif
#if     COM7_EN
    BSP_COM7_RX_PIN,
#endif
#if     COM8_EN
    BSP_COM8_RX_PIN,
#endif
};

static const uint32_t COM_DMA_DR_BASE[COMn] = { 
#if     COM1_EN
    BSP_COM1_DMA_DR_BASE,
#endif
#if     COM2_EN
    BSP_COM2_DMA_DR_BASE,
#endif
#if     COM3_EN
    BSP_COM3_DMA_DR_BASE,
#endif
#if     COM4_EN
    BSP_COM4_DMA_DR_BASE,
#endif
#if     COM5_EN
    BSP_COM5_DMA_DR_BASE,
#endif
#if     COM6_EN
    BSP_COM6_DMA_DR_BASE,
#endif
#if     COM7_EN
    BSP_COM7_DMA_DR_BASE,
#endif
#if     COM8_EN
    BSP_COM8_DMA_DR_BASE,
#endif
};

static const uint32_t COM_DMA_CLK[COMn] = {     
#if     COM1_EN
    BSP_COM1_DMA_CLK,
#endif
#if     COM2_EN
    BSP_COM2_DMA_CLK,
#endif
#if     COM3_EN
    BSP_COM3_DMA_CLK,
#endif
#if     COM4_EN
    BSP_COM4_DMA_CLK,
#endif
#if     COM5_EN
    BSP_COM5_DMA_CLK,
#endif
#if     COM6_EN
    BSP_COM6_DMA_CLK,
#endif
#if     COM7_EN
    BSP_COM7_DMA_CLK,
#endif
#if     COM8_EN
    BSP_COM8_DMA_CLK,
#endif
};

static const uint32_t COM_DMA_TX_FLAG[COMn] = { 
#if     COM1_EN
    BSP_COM1_Tx_DMA_FLAG,
#endif
#if     COM2_EN
    BSP_COM2_Tx_DMA_FLAG,
#endif
#if     COM3_EN
    BSP_COM3_Tx_DMA_FLAG,
#endif
#if     COM4_EN
    BSP_COM4_Tx_DMA_FLAG,
#endif
#if     COM5_EN
    BSP_COM5_Tx_DMA_FLAG,
#endif
#if     COM6_EN
    BSP_COM6_Tx_DMA_FLAG,
#endif
#if     COM7_EN
    BSP_COM7_Tx_DMA_FLAG,
#endif
#if     COM8_EN
    BSP_COM8_Tx_DMA_FLAG,
#endif
};

static const uint32_t COM_DMA_TX_CHANNEL[COMn] = { 
#if     COM1_EN
    BSP_COM1_Tx_DMA_Channel,
#endif
#if     COM2_EN
    BSP_COM2_Tx_DMA_Channel,
#endif
#if     COM3_EN
    BSP_COM3_Tx_DMA_Channel,
#endif
#if     COM4_EN
    BSP_COM4_Tx_DMA_Channel,
#endif
#if     COM5_EN
    BSP_COM5_Tx_DMA_Channel,
#endif
#if     COM6_EN
    BSP_COM6_Tx_DMA_Channel,
#endif
#if     COM7_EN
    BSP_COM7_Tx_DMA_Channel,
#endif
#if     COM8_EN
    BSP_COM8_Tx_DMA_Channel,
#endif
};

static DMA_Stream_TypeDef * const COM_DMA_TX_STREAM[COMn] = { 
#if     COM1_EN
    BSP_COM1_Tx_DMA_Stream,
#endif
#if     COM2_EN
    BSP_COM2_Tx_DMA_Stream,
#endif
#if     COM3_EN
    BSP_COM3_Tx_DMA_Stream,
#endif
#if     COM4_EN
    BSP_COM4_Tx_DMA_Stream,
#endif
#if     COM5_EN
    BSP_COM5_Tx_DMA_Stream,
#endif
#if     COM6_EN
    BSP_COM6_Tx_DMA_Stream,
#endif
#if     COM7_EN
    BSP_COM7_Tx_DMA_Stream,
#endif
#if     COM8_EN
    BSP_COM8_Tx_DMA_Stream,
#endif
};


static const uint32_t COM_RX_IRQn[COMn] = {     
#if     COM1_EN
    BSP_COM1_IRQn,
#endif
#if     COM2_EN
    BSP_COM2_IRQn,
#endif
#if     COM3_EN
    BSP_COM3_IRQn,
#endif
#if     COM4_EN
    BSP_COM4_IRQn,
#endif
#if     COM5_EN
    BSP_COM5_IRQn,
#endif
#if     COM6_EN
    BSP_COM6_IRQn,
#endif
#if     COM7_EN
    BSP_COM7_IRQn,
#endif
#if     COM8_EN
    BSP_COM8_IRQn,
#endif
};

static uint8_t * const COM_TX_BUFF[COMn] = {      
#if     COM1_EN
    COM1_TX_BUFF,
#endif
#if     COM2_EN
    COM2_TX_BUFF,
#endif
#if     COM3_EN
    COM3_TX_BUFF,
#endif
#if     COM4_EN
    COM4_TX_BUFF,
#endif
#if     COM5_EN
    COM5_TX_BUFF,
#endif
#if     COM6_EN
    COM6_TX_BUFF,
#endif
#if     COM7_EN
    COM7_TX_BUFF,
#endif
#if     COM8_EN
    COM8_TX_BUFF,
#endif
};
                                                
static const uint32_t COM_TX_BUFF_SIZE[COMn] = {  
#if     COM1_EN
    BSP_COM1_Tx_BUFF_SIZE,
#endif
#if     COM2_EN
    BSP_COM2_Tx_BUFF_SIZE,
#endif
#if     COM3_EN
    BSP_COM3_Tx_BUFF_SIZE,
#endif
#if     COM4_EN
    BSP_COM4_Tx_BUFF_SIZE,
#endif
#if     COM5_EN
    BSP_COM5_Tx_BUFF_SIZE,
#endif
#if     COM6_EN
    BSP_COM6_Tx_BUFF_SIZE,
#endif
#if     COM7_EN
    BSP_COM7_Tx_BUFF_SIZE,
#endif
#if     COM8_EN
    BSP_COM8_Tx_BUFF_SIZE,
#endif
};     

static uint8_t * const COM_RX_BUFF[COMn] = {      
#if     COM1_EN
    COM1_RX_BUFF,
#endif
#if     COM2_EN
    COM2_RX_BUFF,
#endif
#if     COM3_EN
    COM3_RX_BUFF,
#endif
#if     COM4_EN
    COM4_RX_BUFF,
#endif
#if     COM5_EN
    COM5_RX_BUFF,
#endif
#if     COM6_EN
    COM6_RX_BUFF,
#endif
#if     COM7_EN
    COM7_RX_BUFF,
#endif
#if     COM8_EN
    COM8_RX_BUFF,
#endif
};
                                                
static const uint32_t COM_RX_BUFF_SIZE[COMn] = {  
#if     COM1_EN
    BSP_COM1_Rx_BUFF_SIZE,
#endif
#if     COM2_EN
    BSP_COM2_Rx_BUFF_SIZE,
#endif
#if     COM3_EN
    BSP_COM3_Rx_BUFF_SIZE,
#endif
#if     COM4_EN
    BSP_COM4_Rx_BUFF_SIZE,
#endif
#if     COM5_EN
    BSP_COM5_Rx_BUFF_SIZE,
#endif
#if     COM6_EN
    BSP_COM6_Rx_BUFF_SIZE,
#endif
#if     COM7_EN
    BSP_COM7_Rx_BUFF_SIZE,
#endif
#if     COM8_EN
    BSP_COM8_Rx_BUFF_SIZE,
#endif
};     

static QUEUE8_TYPE * const COM_RX_QUEUE[COMn] = {    
#if     COM1_EN
    &COM1_RX_QUEUE,
#endif
#if     COM2_EN
    &COM2_RX_QUEUE,
#endif
#if     COM3_EN
    &COM3_RX_QUEUE,
#endif
#if     COM4_EN
    &COM4_RX_QUEUE,
#endif
#if     COM5_EN
    &COM5_RX_QUEUE,
#endif
#if     COM6_EN
    &COM6_RX_QUEUE,
#endif
#if     COM7_EN
    &COM7_RX_QUEUE,
#endif
#if     COM8_EN
    &COM8_RX_QUEUE,
#endif
};

static const uint16_t parityArr[3] = {USART_Parity_No, USART_Parity_Odd, USART_Parity_Even};

/*******************************************************************************
* Function Name :BSP_Uart1Open
* Description   :串口及引脚初始化
* Input         :   uartx: COM1,COM2,COM3....
                    baud: 2400，4800，9600，19200，38400，57600，115200
                    data: 数据位数  8,9,
                    stop：停止位数  1,2
                    parity:效验    0: 无效验，1：奇效验，2:偶效验
* Output        :
* Other         :
* Date          :2013.08.12
*******************************************************************************/
void BSP_UartOpen(uint8_t COM,  uint32_t baud, uint8_t data, uint8_t stop, uint8_t parity)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NvicInitdef;
    DMA_InitTypeDef DMA_InitStructure;
    
    /* DMA clock enable */
    RCC_AHB1PeriphClockCmd(COM_DMA_CLK[COM], ENABLE);

    /* Enable GPIO clock */
    RCC_AHB1PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);

    /* Enable UART clock */
    if ((COM_USART[COM] == USART1) || (COM_USART[COM] == USART6))
    {
        RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
    }
    else
    {
        RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);
    }
    USART_DeInit(COM_USART[COM]);
    
    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(COM_TX_PORT[COM], COM_TX_AF_PIN[COM], COM_AF[COM]);
    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(COM_RX_PORT[COM], COM_RX_AF_PIN[COM], COM_AF[COM]);

    /* Configure USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

    /* Configure USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
    GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

    /* USART configuration */
    USART_StructInit(&USART_InitStructure);
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_StopBits = (stop == 2)? USART_StopBits_2 : USART_StopBits_1;
    USART_InitStructure.USART_WordLength = (data == 9)? USART_WordLength_9b : USART_WordLength_8b;
    USART_InitStructure.USART_Parity = (parity < 3)? parityArr[parity] : USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(COM_USART[COM], &USART_InitStructure);
    
    /* Enable USART DMA_TX*/
    USART_DMACmd(COM_USART[COM], USART_DMAReq_Tx, ENABLE);
    
    USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);
    
     /* Enable USART */
    USART_Cmd(COM_USART[COM], ENABLE);   

    NvicInitdef.NVIC_IRQChannel = COM_RX_IRQn[COM];
    NvicInitdef.NVIC_IRQChannelPreemptionPriority = 1;
    NvicInitdef.NVIC_IRQChannelSubPriority = 0;
    NvicInitdef.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init( &NvicInitdef );

    QUEUE_PacketCreate(COM_RX_QUEUE[COM], COM_RX_BUFF[COM], COM_RX_BUFF_SIZE[COM]);
    /* -------------------------------DMA发送------------------------------  */
    /* DMA StreamX Config */
    DMA_DeInit(COM_DMA_TX_STREAM[COM]);    
    
    /* DMA StreamX disable */
    DMA_Cmd(COM_DMA_TX_STREAM[COM], DISABLE);

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_Channel = COM_DMA_TX_CHANNEL[COM];
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)COM_DMA_DR_BASE[COM];
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)COM_TX_BUFF[COM];
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_Init(COM_DMA_TX_STREAM[COM], &DMA_InitStructure);

    DMA_FlowControllerConfig(COM_DMA_TX_STREAM[COM], DMA_FlowCtrl_Memory);    //控制流 取决于缓存大小
}


/*******************************************************************************
* Function Name : void BSP_UartClose(uint8_t COMx)
* Description   : 串口关闭
* Input         : COMx:通道x
* Output        : 
* Other         : 
* Date          : 2013.08.12
*******************************************************************************/
void BSP_UartClose(uint8_t COM)
{
    if ((COM_USART[COM] == USART1) || (COM_USART[COM] == USART6))
    {
        RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], DISABLE); 
    }
    else
    {
        RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], DISABLE);
    }
    
    RCC_AHB1PeriphClockCmd(COM_DMA_CLK[COM], ENABLE);
    DMA_Cmd(COM_DMA_TX_STREAM[COM], DISABLE);
    USART_DMACmd(COM_USART[COM], USART_DMAReq_Tx, DISABLE);
    USART_ITConfig(COM_USART[COM], USART_IT_RXNE, DISABLE);
    USART_Cmd(COM_USART[COM], DISABLE);
}

/*******************************************************************************
* Function Name : uint32_t BSP_UartWrite(uint8_t COMx, uint8_t *buffter, uint32_t len)
* Description   : 串口发送数据
* Input         :   COMx:通道x
* Output        : 
* Other         : 
* Date          : 2013.08.12
*******************************************************************************/
uint32_t BSP_UartWrite(uint8_t COM, uint8_t *buffter, uint32_t len)
{
    if (COM >= COMn)
        return 0;
        
    if (0 == len)
        return 0;
        
    if ((DMA_GetCurrDataCounter(COM_DMA_TX_STREAM[COM]) == 0)
        && (DMA_FIFOStatus_Empty == DMA_GetFIFOStatus(COM_DMA_TX_STREAM[COM])))
    {
        uint32_t resLen = 0;

        USART_ClearFlag(COM_USART[COM], USART_FLAG_TC);
        resLen = (COM_TX_BUFF_SIZE[COM] > len)? len : COM_TX_BUFF_SIZE[COM];
        memcpy((void *)(COM_TX_BUFF[COM]), buffter, resLen);       
        DMA_Cmd(COM_DMA_TX_STREAM[COM], DISABLE);
        DMA_ClearFlag(COM_DMA_TX_STREAM[COM], COM_DMA_TX_FLAG[COM]);
        DMA_SetCurrDataCounter(COM_DMA_TX_STREAM[COM], resLen);                
        DMA_Cmd(COM_DMA_TX_STREAM[COM], ENABLE);

        return resLen;
    }
    return 0;
}

/*******************************************************************************
* Function Name : uint32_t BSP_UartRead(uint8_t COMx, uint8_t *buffter, uint32_t len)
* Description   : 串口读回数据
* Input         : COMx:通道x
* Output        : 
* Other         : 
* Date          : 2013.08.12
*******************************************************************************/
uint32_t BSP_UartRead(uint8_t COM, uint8_t *buffter, uint32_t len)
{
    return QUEUE_PacketOut(COM_RX_QUEUE[COM], buffter, len);
}

/*******************************************************************************
* Function Name : uint32_t BSP_UartTxIdleState(uint8_t COM)
* Description   : 查询 发送是否 发送完成
* Input         : 
* Output        :       1)忙碌      0)空闲
* Other         : 
* Date          : 2013.08.12
*******************************************************************************/
uint32_t BSP_UartTxIdleState(uint8_t COM)
{
    return (DMA_GetCurrDataCounter(COM_DMA_TX_STREAM[COM])
            || (USART_GetFlagStatus(COM_USART[COM], USART_FLAG_TC) != SET));
}


 uint32_t BSP_UartSend(uint8_t COM, uint8_t *buffter, uint32_t len)
{
    uint32_t resLen = 0;

    while(BSP_UartTxIdleState(COM) == 1);

    resLen = BSP_UartWrite(COM,buffter,len);

    return resLen;
}

/*******************************************************************************
* Function Name : void _IRQHandler(uint8_t COM)
* Description   : 中断函数
* Input         : 
* Output        : 
* Other         : 
* Date          : 2013.08.12
*******************************************************************************/
void _IRQHandler(uint8_t COM)
{
    if (USART_GetITStatus(COM_USART[COM], USART_IT_RXNE) != RESET)
    {
        u8 ch = USART_ReceiveData(COM_USART[COM]);  //读数据后 会自动清空标志
        QUEUE_PacketIn(COM_RX_QUEUE[COM], &ch, 1);
    }
}

#if COM1_EN > 0
/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
    _IRQHandler(SCOM1);
}
#endif //COM1_EN > 0

#if COM2_EN > 0
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
    _IRQHandler(COM2);
}
#endif //COM2_EN > 0

#if COM3_EN > 0
/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles USART3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
    _IRQHandler(COM3);
}
#endif //COM3_EN > 0

#if COM4_EN > 0
/*******************************************************************************
* Function Name  : USART4_IRQHandler
* Description    : This function handles USART4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{
    _IRQHandler(COM4);
}
#endif //COM4_EN > 0

#if COM5_EN > 0
/*******************************************************************************
* Function Name  : USART5_IRQHandler
* Description    : This function handles USART5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{
    _IRQHandler(COM5);
}
#endif //COM5_EN > 0

#if COM6_EN > 0
/*******************************************************************************
* Function Name  : USART6_IRQHandler
* Description    : This function handles USART6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART6_IRQHandler(void)
{
    _IRQHandler(COM6);
}
#endif //COM6_EN > 0

#if COM7_EN > 0
/*******************************************************************************
* Function Name  : USART7_IRQHandler
* Description    : This function handles USART7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART7_IRQHandler(void)
{
    _IRQHandler(COM7);
}
#endif //COM7_EN > 0

#if COM8_EN > 0
/*******************************************************************************
* Function Name  : USART8_IRQHandler
* Description    : This function handles USART8 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART8_IRQHandler(void)
{
    _IRQHandler(COM8);
}
#endif //COM8_EN > 0

