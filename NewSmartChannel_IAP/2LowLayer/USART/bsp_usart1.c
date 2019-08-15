/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : drv_usart1.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月22日
  最近修改   :
  功能描述   : 串口1初始化
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月22日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/
#include "stm32f4xx_conf.h"
#include "bsp_usart1.h"

//Private Function Prototype
static uint32_t drv_Usart1_ReadByte(uint8_t *key);

//USART2 get a byte DR
//key: pointer to store data
//return: 
//      0:fail
//      1:success
static uint32_t drv_Usart1_ReadByte(uint8_t *key)
{

  if ( USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
  {
    *key = (uint8_t)USART1->DR;
    return 1;
  }
  else
  {
    return 0;
  }
}


//USART2 get a byte from HyperTerminal
//return: Rx byte
uint8_t bsp_Usart1_GetByte(void)
{
  uint8_t key = 0;

  /* Waiting for user input */
  while (1)
  {
    if (drv_Usart1_ReadByte((uint8_t*)&key)) break;
    
  }
  return key;

}

//Private Function Prototype
//DMA2 for usart1 configuration
static void Usart1_DMA2_Config(void)  
{  
    uint8_t dummy_data = 0;

    DMA_InitTypeDef DMA_InitStructure;
    

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2时钟使能 
    
    //reset 
    DMA_DeInit(DMA2_Stream7); //如果使用串口1，只能 DMA2 通道4，数据流7
    
    //DMA configuration  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4; //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; //DMA外设地址
    DMA_InitStructure.DMA_BufferSize = 1;   //数据传输量，初始化时先定为1
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&dummy_data; //DMA 存储器0地址     
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; //存储器到外设模式    
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //存储器增量模式 
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //外设数据长度:8位 
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;  // 使用普通模式    
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //中等优先级    
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  //存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //外设突发单次传输
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold =DMA_FIFOThreshold_Full; 
                
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);    //初始化DMA Stream     
    DMA_Cmd(DMA2_Stream7,ENABLE);                  //开启DMA传输 
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); //使能串口1的DMA发送  
}

//usart1 send data via DMA
//@ int8_t *buf: data buffer ready to send
//@ int16_t len: data length
void bsp_Usart1_DMA_Send(uint8_t *buf, uint16_t len)
{  
    DMA_InitTypeDef DMA_InitStructure;
    //wait unitl last package is sent
    while(DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7)==RESET); //等待DMA可配置 
    DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7);
    //DMA configuration  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; 
    DMA_InitStructure.DMA_BufferSize = len;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)buf;      
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;     
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;     
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;  
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; 
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold =DMA_FIFOThreshold_HalfFull; 
                
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);         //初始化DMA Stream   
    DMA_Cmd(DMA2_Stream7,ENABLE);                       //开启DMA传输
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);      //使能串口1的DMA发送
}

//usart2 configuration
//default setting is 8,n,1
//@ int baudrate: the desired baudrate
void bsp_Usart1_Init(int BaudRate)
{
     //GPIO端口设置
     GPIO_InitTypeDef GPIO_InitStructure;
     USART_InitTypeDef USART_InitStructure;
//     NVIC_InitTypeDef NVIC_InitStructure;
    
     RCC_AHB1PeriphClockCmd(USART1_GPIO_CLK,ENABLE); //使能GPIOA时钟
     RCC_APB2PeriphClockCmd(USART1_CLK,ENABLE);//使能USART1时钟
    
     //串口1对应引脚复用映射
     GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF); //GPIOA9复用为USART1
     GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF); //GPIOA10复用为USART1
     
     //USART1端口配置
     GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN; //GPIOA9与GPIOA10
     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
     GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //速度50MHz
     GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
     GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
     GPIO_Init(USART1_GPIO_PORT,&GPIO_InitStructure); //初始化PA9，PA10
    
    //USART1 初始化设置
     USART_InitStructure.USART_BaudRate = BaudRate;//波特率设置
     USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
     USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
     USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
     USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
     USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
     USART_Init(USART1, &USART_InitStructure); //初始化串口1       
     USART_Cmd(USART1, ENABLE);  //使能串口1   
	 
    //Usart1 NVIC 配置
//    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;       //子优先级3
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
//    NVIC_Init(&NVIC_InitStructure); //根据指定的参数初始化VIC寄存器    
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断    


	//使用USART1 DMA 
	Usart1_DMA2_Config();	
}







