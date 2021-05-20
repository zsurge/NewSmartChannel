/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : bsp_dma_usart1.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��22��
  ����޸�   :
  ��������   : ����1��ʼ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/
#include "bsp_dma_usart1.h"
/* FreeRTOSͷ�ļ� */
#include "FreeRTOS.h"
#include "task.h"

volatile uint8_t USART1_DMA_RecvBuf[USART1_DMA_MAX_SIZE] = {0};
volatile uint16_t RecvTop1 = 0;
volatile uint16_t RecvEnd1 = 0;


#define UART1_RX_LEN 256
#define UART1_TX_LEN USART1_DMA_MAX_SIZE


uint8_t Uart1_Tx[UART1_TX_LEN] = {0}; //����1����DMA����     
uint8_t Uart1_Rx[UART1_RX_LEN] = {0}; //����1����DMA���� 




static void BSP_DMAUsar1Rx_Init(void);
static void BSP_DMAUsar1Tx_Init(void);

/*****************************************************************************
 �� �� ��  : bsp_Usart1Init
 ��������  : ���ڳ�ʼ��
 �������  : uint32_t BaudRate  ������
 �������  : ��
 �� �� ֵ  : 
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��22��
    ��    ��   : �Ŷ�
void bsp_Usart1Init (uint32_t BaudRate);
    �޸�����   : �����ɺ���

*****************************************************************************/
void bsp_Usart1_Init (uint32_t BaudRate)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(USART1_GPIO_CLK,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(USART1_CLK,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	//���� PXx �� USARTx_Tx
	GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_TX_SOURCE,USART1_TX_AF); //GPIOB 6����ΪUSART1
	//���� PXx �� USARTx__Rx
	GPIO_PinAFConfig(USART1_GPIO_PORT,USART1_RX_SOURCE,USART1_RX_AF); //GPIOB 7����ΪUSART1
	
	//USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN | USART1_RX_PIN; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(USART1_GPIO_PORT,&GPIO_InitStructure); //��ʼ��PB6��PB7

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = BaudRate;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1  

	//���������ɱ�־	
    USART_ClearFlag(USART1, USART_FLAG_TC);
    
    //����USART1���߿����ж�
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);  
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE); 
	USART_ITConfig(USART1,USART_IT_TXE,DISABLE); 
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE); 

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =6;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
    //���ڽ���DMA����
    BSP_DMAUsar1Rx_Init();
    //���ڷ���DMA����
    BSP_DMAUsar1Tx_Init();

    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //����DMA��ʽ����   
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); //����DMA��ʽ����

    //����USART1
    USART_Cmd(USART1, ENABLE);    
    
    //Create_RingBuff(&ringbuff_handle,  gUsart1RXBuff,sizeof(gUsart1RXBuff));
}

void USART1_IRQHandler(void)
{    
    uint32_t rxBuffLen = 0;
    uint32_t ulReturn;
    int i = 0;
    /* �����ٽ�Σ��ٽ�ο���Ƕ�� */
    ulReturn = taskENTER_CRITICAL_FROM_ISR();
    
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
    {
      //1.���USART6��������ж�
        USART1->SR;  
        USART1->DR;   //��USART_IT_IDLE��־  
      //2.�洢�յ����������ݡ����ȡ���־λ
        DMA_Cmd(DMA2_Stream5,DISABLE); //ʹ��������1 ͨ��5
        
        DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_FEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_HTIF5);//�����־λ
    
        rxBuffLen = UART1_RX_LEN - DMA_GetCurrDataCounter(DMA2_Stream5);        //����������ݰ�����    


        //�����յ������ݰ�����
        for (i = 0;i < rxBuffLen;i++)                           
        {  
    		if ( (RecvEnd1 == (RecvTop1 - 1) ) || ( (RecvTop1 == 0) && (RecvEnd1 == (USART1_DMA_MAX_SIZE - 1) ) ) )
    		{
    			//�������
    			RecvTop1 = 0;
    			RecvEnd1 = 0;
    		}
    		else
    		{
    			USART1_DMA_RecvBuf[RecvEnd1] = Uart1_Rx[i];;
    			RecvEnd1++;

    			if (RecvEnd1 >= USART1_DMA_MAX_SIZE) 
    			{
    			    RecvEnd1 = 0;
    			}
    		}    		
        }
        
        //Write_RingBuff(&ringbuff_handle,Uart1_Rx,rxBuffLen); 
        DMA_SetCurrDataCounter(DMA2_Stream5,UART1_RX_LEN);                      //���ô������ݳ���
        DMA_Cmd(DMA2_Stream5,ENABLE);                                           //��DMA 
    } 

    if(USART_GetITStatus(USART1, USART_IT_TC) != RESET)
    {
        USART_ClearITPendingBit(USART1, USART_IT_TC);   //���USART1��������жϱ�־
        DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7 | DMA_FLAG_FEIF7 | 
                      DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7);//�����־λ                     
    }    

  /* �˳��ٽ�� */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );	
}

//USART1����DMA����
static void BSP_DMAUsar1Rx_Init(void)
{
    DMA_InitTypeDef   DMA_InitStructure;
    u16 mid_u16RetryCnt = 0;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);                        //����DMAʱ��  
    
    DMA_DeInit(DMA2_Stream5);
    while ((DMA_GetCmdStatus(DMA2_Stream5) != DISABLE) && (mid_u16RetryCnt++ < 500));
    
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);         //�����ַ      
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart1_Rx;                 //�ڴ��ַ      
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                     //dma���䷽����      
    DMA_InitStructure.DMA_BufferSize = UART1_RX_LEN;                            //����DMA�ڴ���ʱ�������ĳ���     
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //����DMA���������ģʽ��һ������        
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //����DMA���ڴ����ģʽ      
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //���������ֳ�         
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //�ڴ������ֳ�      
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //����DMA�Ĵ���ģʽ      
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                     //����DMA�����ȼ���     
    
 	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
 	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
 	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
 	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream5,&DMA_InitStructure);   //ʹ��������2 ͨ��5
    DMA_Cmd(DMA2_Stream5, ENABLE);  //����DMA����
}
 
//USART6����DMA����
static void BSP_DMAUsar1Tx_Init(void)
{
    DMA_InitTypeDef   DMA_InitStructure;  
    NVIC_InitTypeDef NVIC_InitStructure;
    u16 mid_u16RetryCnt = 0;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);                        //����DMAʱ��  
    
    DMA_DeInit(DMA2_Stream7);
    while ((DMA_GetCmdStatus(DMA2_Stream7) != DISABLE) && (mid_u16RetryCnt++ < 500));
    
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);         //DMA�������ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Uart1_Tx;                 //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                     //���ݴ��䷽�򣬴��ڴ��ȡ���͵�����
	DMA_InitStructure.DMA_BufferSize = UART1_TX_LEN;                            //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                     //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;     //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;             //���ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                               //����������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                       //DMAͨ�� xӵ�������ȼ� 
    
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;    
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);                                 //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ���   

    //DMA�����ж�����
    NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);   
    DMA_Cmd(DMA2_Stream7,DISABLE);
}

//USART2 DMA����ָ�����ȵ�����
//str��Ҫ���͵������׵�ַ
//cndtr:���ݴ����� 
void bsp_DMAUsart1Send(uint8_t *str,uint8_t cndtr)
{
//    u16 l_u16RetryCnt = 0; 
//    memcpy(Uart1_Tx, str, cndtr);
//    DMA_Cmd(DMA2_Stream7, DISABLE);                      //�ر�DMA����           
//    while ((DMA_GetCmdStatus(DMA2_Stream7) != DISABLE) && (l_u16RetryCnt++ < 500));	//�ȴ�DMA������	
//    DMA_SetCurrDataCounter(DMA2_Stream7, cndtr);  //���ݴ����� 	 
//    DMA_Cmd(DMA2_Stream7, ENABLE);                      	//����DMA���� 

    memcpy(Uart1_Tx, str, cndtr);
    USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);                                //ʹ�ܴ���6��DMA����
	DMA_Cmd(DMA2_Stream7, DISABLE );                                            //�ر�USART6 TX DMA2 ��ָʾ��ͨ��      
 	DMA_SetCurrDataCounter(DMA2_Stream7,cndtr);                                 //DMAͨ����DMA����Ĵ�С
 	DMA_Cmd(DMA2_Stream7, ENABLE);    
}

//DMA2�жϺ�����
void DMA2_Stream7_IRQHandler(void)
{
    if(DMA_GetFlagStatus(DMA2_Stream7, DMA_FLAG_TCIF7) != RESET)	//DMA�������  
    {   
		DMA_ClearFlag(DMA2_Stream7, DMA_FLAG_TCIF7 | DMA_FLAG_FEIF7 | 
					  DMA_FLAG_DMEIF7 | DMA_FLAG_TEIF7 | DMA_FLAG_HTIF7); 	//�����־λ			
		
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));	//�ȴ�USART1������ɱ�־TC��1
		
		USART_ClearFlag(USART1, USART_FLAG_TC); 	//���������ɱ�־

		DMA_Cmd(DMA2_Stream7,DISABLE);
		
		//�򿪷�������ж�,������������ֽ�
        USART_ITConfig(USART1,USART_IT_TC,ENABLE);            

        printf("DMA SEND END\r\n");
    } 
}


uint8_t bsp_DMAUsart1ReadOne (uint8_t *Str)
{
	if (RecvTop1 == RecvEnd1) return 0;//read nothing

	*Str = USART1_DMA_RecvBuf[RecvTop1];
	RecvTop1++;

	if (RecvTop1 >= USART1_DMA_MAX_SIZE) RecvTop1 = 0;

	return 1;//read one
}



uint16_t bsp_DMAUsart1Read(uint8_t *buffter, uint32_t len)
{    
	uint16_t RecvLen = 0;
	uint8_t tmp[1] = {0};

	if (len == 0 || buffter == NULL) return 0;

	while (len--)
	{
		if (bsp_DMAUsart1ReadOne (tmp) == 1)
		{
			buffter[RecvLen++] = tmp[0];
		}

		if (RecvLen >= len) return RecvLen;
	}

	return RecvLen;
}




