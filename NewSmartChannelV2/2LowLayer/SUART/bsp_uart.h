 /*
 ********************************************************************************
 *
 *                                 BSP_Uart.h
 *
 * File          : BSP_Uart.h
 * Version       : V1.0
 * Author        : whq
 * Mode          : Thumb2
 * Toolchain     : 
 * Description   : 串口驱动头文件
 *                
 * History       :
 * Date          : 2013.08.12
 *******************************************************************************/
 
 #ifndef _BSP_UART_H_
 #define _BSP_UART_H_
 
 #include <stdint.h>
 
 
 #define COM1_EN         1
 #define COM2_EN         0
 #define COM3_EN         0
 #define COM4_EN         0
 #define COM5_EN         0
 #define COM6_EN         0
 #define COM7_EN         0
 #define COM8_EN         0
 
 
 
 #define COM_1_1                             //映射:COM_1_0，映射1:COM_1_1
 #define COM_2_0                             //映射:COM_2_0，映射1:COM_2_1
 #define COM_3_2                             //映射:COM_3_2，
 #define COM_4_0 
 #define COM_5_0 
 #define COM_6_0 
 #define COM_7_0 
 #define COM_8_0 
 
 
 
 #if !(COM1_EN || COM2_EN || COM3_EN || COM4_EN || COM5_EN || COM6_EN || COM7_EN || COM8_EN)
 #error  "请至少使能一路串口!"
 #endif
 
 
 typedef enum {
 #if     COM1_EN
         SCOM1,
 #endif
 #if     COM2_EN
         COM2,
 #endif
 #if     COM3_EN
         COM3,
 #endif
 #if     COM4_EN
         COM4,
 #endif
 #if     COM5_EN
         COM5,
 #endif
 #if     COM6_EN
         COM6,
 #endif
 #if     COM7_EN
         COM7,
 #endif
 #if     COM8_EN
         COM8,
 #endif
     COM_MAX
 }COM_PORT;
 
 /** @addtogroup STM3210E_BSP_LOW_LEVEL_COM
   * @{
   */
 #define COMn                                COM_MAX
 
 
 
 /**
  * @brief Definition for COM port1, connected to USART1
  */ 
 #define BSP_COM1                            USART1
 #define BSP_COM1_CLK                        RCC_APB2Periph_USART1
 #define BSP_COM1_AF                         GPIO_AF_USART1
 #ifdef COM_1_3
 #define BSP_COM1_TX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM1_TX_PIN                     GPIO_Pin_6
 #define BSP_COM1_TX_GPIO_PORT               GPIOB
 #define BSP_COM1_TX_GPIO_CLK                RCC_AHB1Periph_GPIOB
 #define BSP_COM1_RX_AF_PIN                  GPIO_PinSource10
 #define BSP_COM1_RX_PIN                     GPIO_Pin_10
 #define BSP_COM1_RX_GPIO_PORT               GPIOA
 #define BSP_COM1_RX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #elif defined(COM_1_2)
 #define BSP_COM1_TX_AF_PIN                  GPIO_PinSource9
 #define BSP_COM1_TX_PIN                     GPIO_Pin_9
 #define BSP_COM1_TX_GPIO_PORT               GPIOA
 #define BSP_COM1_TX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #define BSP_COM1_RX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM1_RX_PIN                     GPIO_Pin_7
 #define BSP_COM1_RX_GPIO_PORT               GPIOB
 #define BSP_COM1_RX_GPIO_CLK                RCC_AHB1Periph_GPIOB
 #elif defined(COM_1_1)
 #define BSP_COM1_TX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM1_TX_PIN                     GPIO_Pin_6
 #define BSP_COM1_TX_GPIO_PORT               GPIOB
 #define BSP_COM1_TX_GPIO_CLK                RCC_AHB1Periph_GPIOB
 #define BSP_COM1_RX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM1_RX_PIN                     GPIO_Pin_7
 #define BSP_COM1_RX_GPIO_PORT               GPIOB
 #define BSP_COM1_RX_GPIO_CLK                RCC_AHB1Periph_GPIOB
 #else
 #define BSP_COM1_TX_AF_PIN                  GPIO_PinSource9
 #define BSP_COM1_TX_PIN                     GPIO_Pin_9
 #define BSP_COM1_TX_GPIO_PORT               GPIOA
 #define BSP_COM1_TX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #define BSP_COM1_RX_AF_PIN                  GPIO_PinSource10
 #define BSP_COM1_RX_PIN                     GPIO_Pin_10
 #define BSP_COM1_RX_GPIO_PORT               GPIOA
 #define BSP_COM1_RX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #endif
 
 #define BSP_COM1_IRQn                       USART1_IRQn
 
 #define BSP_COM1_DMA_DR_BASE                0x40011004
 #define BSP_COM1_DMA_CLK                    RCC_AHB1Periph_DMA2
 #define BSP_COM1_Tx_DMA_FLAG                DMA_FLAG_TCIF7
 #define BSP_COM1_Tx_DMA_Stream              DMA2_Stream7
 #define BSP_COM1_Tx_DMA_Channel             DMA_Channel_4
 
 #define BSP_COM1_Tx_BUFF_SIZE               0x600       //发送缓冲区大小
 #define BSP_COM1_Rx_BUFF_SIZE               0x200         //接收缓冲区大小
 
 
 /**
  * @brief Definition for COM port2, connected to USART2
  */ 
 #define BSP_COM2                            USART2
 #define BSP_COM2_CLK                        RCC_APB1Periph_USART2
 #define BSP_COM2_AF                         GPIO_AF_USART2
 
 #ifdef COM_2_3
 #define BSP_COM2_TX_AF_PIN                  GPIO_PinSource2
 #define BSP_COM2_TX_PIN                     GPIO_Pin_2
 #define BSP_COM2_TX_GPIO_PORT               GPIOA
 #define BSP_COM2_TX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #define BSP_COM2_RX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM2_RX_PIN                     GPIO_Pin_6
 #define BSP_COM2_RX_GPIO_PORT               GPIOD
 #define BSP_COM2_RX_GPIO_CLK                RCC_AHB1Periph_GPIOD
 #elif defined(COM_2_2)
 #define BSP_COM2_TX_AF_PIN                  GPIO_PinSource5
 #define BSP_COM2_TX_PIN                     GPIO_Pin_5
 #define BSP_COM2_TX_GPIO_PORT               GPIOD
 #define BSP_COM2_TX_GPIO_CLK                RCC_AHB1Periph_GPIOD
 #define BSP_COM2_RX_AF_PIN                  GPIO_PinSource3
 #define BSP_COM2_RX_PIN                     GPIO_Pin_3
 #define BSP_COM2_RX_GPIO_PORT               GPIOA
 #define BSP_COM2_RX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #elif defined(COM_2_1)
 #define BSP_COM2_TX_AF_PIN                  GPIO_PinSource5
 #define BSP_COM2_TX_PIN                     GPIO_Pin_5
 #define BSP_COM2_TX_GPIO_PORT               GPIOD
 #define BSP_COM2_TX_GPIO_CLK                RCC_AHB1Periph_GPIOD
 #define BSP_COM2_RX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM2_RX_PIN                     GPIO_Pin_6
 #define BSP_COM2_RX_GPIO_PORT               GPIOD
 #define BSP_COM2_RX_GPIO_CLK                RCC_AHB1Periph_GPIOD
 #else
 #define BSP_COM2_TX_AF_PIN                  GPIO_PinSource2
 #define BSP_COM2_TX_PIN                     GPIO_Pin_2
 #define BSP_COM2_TX_GPIO_PORT               GPIOA
 #define BSP_COM2_TX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #define BSP_COM2_RX_AF_PIN                  GPIO_PinSource3
 #define BSP_COM2_RX_PIN                     GPIO_Pin_3
 #define BSP_COM2_RX_GPIO_PORT               GPIOA
 #define BSP_COM2_RX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #endif
     
 #define BSP_COM2_IRQn                       USART2_IRQn
     
 #define BSP_COM2_DMA_DR_BASE                0x40004404
 #define BSP_COM2_DMA_CLK                    RCC_AHB1Periph_DMA1
 #define BSP_COM2_Tx_DMA_FLAG                DMA_FLAG_TCIF6
 #define BSP_COM2_Tx_DMA_Stream              DMA1_Stream6
 #define BSP_COM2_Tx_DMA_Channel             DMA_Channel_4
 
 #define BSP_COM2_Tx_BUFF_SIZE               0x400       //发送缓冲区大小
 #define BSP_COM2_Rx_BUFF_SIZE               0x400       //接收缓冲区大小
 
 
 
 /**
  * @brief Definition for COM port3, connected to USART3
  */ 
 #define BSP_COM3                            USART3
 #define BSP_COM3_CLK                        RCC_APB1Periph_USART3
 #define BSP_COM3_AF                         GPIO_AF_USART3
 #ifdef COM_3_3                              //自由组合部分
 #define BSP_COM3_TX_AF_PIN                  GPIO_PinSource8
 #define BSP_COM3_TX_PIN                     GPIO_Pin_8
 #define BSP_COM3_TX_GPIO_PORT               GPIOD
 #define BSP_COM3_TX_GPIO_CLK                RCC_AHB1Periph_GPIOD
 #define BSP_COM3_RX_AF_PIN                  GPIO_PinSource11
 #define BSP_COM3_RX_PIN                     GPIO_Pin_11
 #define BSP_COM3_RX_GPIO_PORT               GPIOC
 #define BSP_COM3_RX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #elif defined(COM_3_2)
 #define BSP_COM3_TX_AF_PIN                  GPIO_PinSource8
 #define BSP_COM3_TX_PIN                     GPIO_Pin_8
 #define BSP_COM3_TX_GPIO_PORT               GPIOD
 #define BSP_COM3_TX_GPIO_CLK                RCC_AHB1Periph_GPIOD
 #define BSP_COM3_RX_AF_PIN                  GPIO_PinSource9
 #define BSP_COM3_RX_PIN                     GPIO_Pin_9
 #define BSP_COM3_RX_GPIO_PORT               GPIOD
 #define BSP_COM3_RX_GPIO_CLK                RCC_AHB1Periph_GPIOD
 #elif defined(COM_3_1)
 #define BSP_COM3_TX_AF_PIN                  GPIO_PinSource10
 #define BSP_COM3_TX_PIN                     GPIO_Pin_10
 #define BSP_COM3_TX_GPIO_PORT               GPIOC
 #define BSP_COM3_TX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #define BSP_COM3_RX_AF_PIN                  GPIO_PinSource11
 #define BSP_COM3_RX_PIN                     GPIO_Pin_11
 #define BSP_COM3_RX_GPIO_PORT               GPIOC
 #define BSP_COM3_RX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #else
 #define BSP_COM3_TX_AF_PIN                  GPIO_PinSource10
 #define BSP_COM3_TX_PIN                     GPIO_Pin_10
 #define BSP_COM3_TX_GPIO_PORT               GPIOB
 #define BSP_COM3_TX_GPIO_CLK                RCC_AHB1Periph_GPIOB
 #define BSP_COM3_RX_AF_PIN                  GPIO_PinSource11
 #define BSP_COM3_RX_PIN                     GPIO_Pin_11
 #define BSP_COM3_RX_GPIO_PORT               GPIOB
 #define BSP_COM3_RX_GPIO_CLK                RCC_AHB1Periph_GPIOB
 #endif
     
 #define BSP_COM3_IRQn                       USART3_IRQn
     
 #define BSP_COM3_DMA_DR_BASE                0x40004804
 #define BSP_COM3_DMA_CLK                    RCC_AHB1Periph_DMA1
 #define BSP_COM3_Tx_DMA_FLAG                DMA_FLAG_TCIF3
 #define BSP_COM3_Tx_DMA_Stream              DMA1_Stream3
 #define BSP_COM3_Tx_DMA_Channel             DMA_Channel_4
 
 #define BSP_COM3_Tx_BUFF_SIZE               0x400       //发送缓冲区大小
 #define BSP_COM3_Rx_BUFF_SIZE               0x400       //接收缓冲区大小
 
 
 /**
  * @brief Definition for COM port4, connected to USART4
  */ 
 #define BSP_COM4                            UART4
 #define BSP_COM4_CLK                        RCC_APB1Periph_UART4
 #define BSP_COM4_AF                         GPIO_AF_UART4
 #ifdef COM_4_3
 #define BSP_COM4_TX_AF_PIN                  GPIO_PinSource10
 #define BSP_COM4_TX_PIN                     GPIO_Pin_10
 #define BSP_COM4_TX_GPIO_PORT               GPIOC
 #define BSP_COM4_TX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #define BSP_COM4_RX_AF_PIN                  GPIO_PinSource1
 #define BSP_COM4_RX_PIN                     GPIO_Pin_1
 #define BSP_COM4_RX_GPIO_PORT               GPIOA
 #define BSP_COM4_RX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #elif defined(COM_4_2)
 #define BSP_COM4_TX_AF_PIN                  GPIO_PinSource0
 #define BSP_COM4_TX_PIN                     GPIO_Pin_0
 #define BSP_COM4_TX_GPIO_PORT               GPIOA
 #define BSP_COM4_TX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #define BSP_COM4_RX_AF_PIN                  GPIO_PinSource11
 #define BSP_COM4_RX_PIN                     GPIO_Pin_11
 #define BSP_COM4_RX_GPIO_PORT               GPIOC
 #define BSP_COM4_RX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #elif defined(COM_4_1)
 #define BSP_COM4_TX_AF_PIN                  GPIO_PinSource10
 #define BSP_COM4_TX_PIN                     GPIO_Pin_10
 #define BSP_COM4_TX_GPIO_PORT               GPIOC
 #define BSP_COM4_TX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #define BSP_COM4_RX_AF_PIN                  GPIO_PinSource11
 #define BSP_COM4_RX_PIN                     GPIO_Pin_11
 #define BSP_COM4_RX_GPIO_PORT               GPIOC
 #define BSP_COM4_RX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #else
 #define BSP_COM4_TX_AF_PIN                  GPIO_PinSource0
 #define BSP_COM4_TX_PIN                     GPIO_Pin_0
 #define BSP_COM4_TX_GPIO_PORT               GPIOA
 #define BSP_COM4_TX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #define BSP_COM4_RX_AF_PIN                  GPIO_PinSource1
 #define BSP_COM4_RX_PIN                     GPIO_Pin_1
 #define BSP_COM4_RX_GPIO_PORT               GPIOA
 #define BSP_COM4_RX_GPIO_CLK                RCC_AHB1Periph_GPIOA
 #endif
         
 #define BSP_COM4_IRQn                       UART4_IRQn
         
 #define BSP_COM4_DMA_DR_BASE                0x40004C04
 #define BSP_COM4_DMA_CLK                    RCC_AHB1Periph_DMA1
 #define BSP_COM4_Tx_DMA_FLAG                DMA_FLAG_TCIF4
 #define BSP_COM4_Tx_DMA_Stream              DMA1_Stream4
 #define BSP_COM4_Tx_DMA_Channel             DMA_Channel_4
 
 #define BSP_COM4_Tx_BUFF_SIZE               0x400       //发送缓冲区大小
 #define BSP_COM4_Rx_BUFF_SIZE               0x400       //接收缓冲区大小
 
 
 /**
  * @brief Definition for COM port5, connected to USART5
  */ 
 #define BSP_COM5                            UART5
 #define BSP_COM5_CLK                        RCC_APB1Periph_UART5
 #define BSP_COM5_AF                         GPIO_AF_UART5
 
 #define BSP_COM5_TX_AF_PIN                  GPIO_PinSource12
 #define BSP_COM5_TX_PIN                     GPIO_Pin_12
 #define BSP_COM5_TX_GPIO_PORT               GPIOC
 #define BSP_COM5_TX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #define BSP_COM5_RX_AF_PIN                  GPIO_PinSource2
 #define BSP_COM5_RX_PIN                     GPIO_Pin_2
 #define BSP_COM5_RX_GPIO_PORT               GPIOD
 #define BSP_COM5_RX_GPIO_CLK                RCC_AHB1Periph_GPIOD
         
 #define BSP_COM5_IRQn                       UART5_IRQn
         
 #define BSP_COM5_DMA_DR_BASE                0x40005004
 #define BSP_COM5_DMA_CLK                    RCC_AHB1Periph_DMA1
 #define BSP_COM5_Tx_DMA_FLAG                DMA_FLAG_TCIF7
 #define BSP_COM5_Tx_DMA_Stream              DMA1_Stream7
 #define BSP_COM5_Tx_DMA_Channel             DMA_Channel_4
 
 #define BSP_COM5_Tx_BUFF_SIZE               125     //发送缓冲区大小
 #define BSP_COM5_Rx_BUFF_SIZE               125     //接收缓冲区大小
 
 
 /**
  * @brief Definition for COM port6, connected to USART6
  */ 
 #define BSP_COM6                            USART6
 #define BSP_COM6_CLK                        RCC_APB2Periph_USART6
 #define BSP_COM6_AF                         GPIO_AF_USART6
 #ifdef COM_6_3
 #define BSP_COM6_TX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM6_TX_PIN                     GPIO_Pin_6
 #define BSP_COM6_TX_GPIO_PORT               GPIOC
 #define BSP_COM6_TX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #define BSP_COM6_RX_AF_PIN                  GPIO_PinSource9
 #define BSP_COM6_RX_PIN                     GPIO_Pin_9
 #define BSP_COM6_RX_GPIO_PORT               GPIOG
 #define BSP_COM6_RX_GPIO_CLK                RCC_AHB1Periph_GPIOG
 #elif defined(COM_6_2)
 #define BSP_COM6_TX_AF_PIN                  GPIO_PinSource14
 #define BSP_COM6_TX_PIN                     GPIO_Pin_14
 #define BSP_COM6_TX_GPIO_PORT               GPIOG
 #define BSP_COM6_TX_GPIO_CLK                RCC_AHB1Periph_GPIOG
 #define BSP_COM6_RX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM6_RX_PIN                     GPIO_Pin_7
 #define BSP_COM6_RX_GPIO_PORT               GPIOC
 #define BSP_COM6_RX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #elif defined(COM_6_1)
 #define BSP_COM6_TX_AF_PIN                  GPIO_PinSource14
 #define BSP_COM6_TX_PIN                     GPIO_Pin_14
 #define BSP_COM6_TX_GPIO_PORT               GPIOG
 #define BSP_COM6_TX_GPIO_CLK                RCC_AHB1Periph_GPIOG
 #define BSP_COM6_RX_AF_PIN                  GPIO_PinSource9
 #define BSP_COM6_RX_PIN                     GPIO_Pin_9
 #define BSP_COM6_RX_GPIO_PORT               GPIOG
 #define BSP_COM6_RX_GPIO_CLK                RCC_AHB1Periph_GPIOG
 #else
 #define BSP_COM6_TX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM6_TX_PIN                     GPIO_Pin_6
 #define BSP_COM6_TX_GPIO_PORT               GPIOC
 #define BSP_COM6_TX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #define BSP_COM6_RX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM6_RX_PIN                     GPIO_Pin_7
 #define BSP_COM6_RX_GPIO_PORT               GPIOC
 #define BSP_COM6_RX_GPIO_CLK                RCC_AHB1Periph_GPIOC
 #endif
         
 #define BSP_COM6_IRQn                       USART6_IRQn
         
 #define BSP_COM6_DMA_DR_BASE                0x40011404
 #define BSP_COM6_DMA_CLK                    RCC_AHB1Periph_DMA2
 #define BSP_COM6_Tx_DMA_FLAG                DMA_FLAG_TCIF6
 #define BSP_COM6_Tx_DMA_Stream              DMA2_Stream6
 #define BSP_COM6_Tx_DMA_Channel             DMA_Channel_5
 
         
 #define BSP_COM6_Tx_BUFF_SIZE               0x400       //发送缓冲区大小
 #define BSP_COM6_Rx_BUFF_SIZE               0x400       //接收缓冲区大小
 
 /**
  * @brief Definition for COM port7, connected to USART7
  */ 
 #define BSP_COM7                            UART7
 #define BSP_COM7_CLK                        RCC_APB1Periph_UART7
 #define BSP_COM7_AF                         GPIO_AF_UART7
 #ifdef COM_7_3
 #define BSP_COM7_TX_AF_PIN                  GPIO_PinSource8
 #define BSP_COM7_TX_PIN                     GPIO_Pin_8
 #define BSP_COM7_TX_GPIO_PORT               GPIOE
 #define BSP_COM7_TX_GPIO_CLK                RCC_AHB1Periph_GPIOE
 #define BSP_COM7_RX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM7_RX_PIN                     GPIO_Pin_6
 #define BSP_COM7_RX_GPIO_PORT               GPIOF
 #define BSP_COM7_RX_GPIO_CLK                RCC_AHB1Periph_GPIOF
 #elif defined(COM_7_2)
 #define BSP_COM7_TX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM7_TX_PIN                     GPIO_Pin_7
 #define BSP_COM7_TX_GPIO_PORT               GPIOF
 #define BSP_COM7_TX_GPIO_CLK                RCC_AHB1Periph_GPIOF
 #define BSP_COM7_RX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM7_RX_PIN                     GPIO_Pin_7
 #define BSP_COM7_RX_GPIO_PORT               GPIOE
 #define BSP_COM7_RX_GPIO_CLK                RCC_AHB1Periph_GPIOE
 #elif defined(COM_7_1)
 #define BSP_COM7_TX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM7_TX_PIN                     GPIO_Pin_7
 #define BSP_COM7_TX_GPIO_PORT               GPIOF
 #define BSP_COM7_TX_GPIO_CLK                RCC_AHB1Periph_GPIOF
 #define BSP_COM7_RX_AF_PIN                  GPIO_PinSource6
 #define BSP_COM7_RX_PIN                     GPIO_Pin_6
 #define BSP_COM7_RX_GPIO_PORT               GPIOF
 #define BSP_COM7_RX_GPIO_CLK                RCC_AHB1Periph_GPIOF
 #else
 #define BSP_COM7_TX_AF_PIN                  GPIO_PinSource8
 #define BSP_COM7_TX_PIN                     GPIO_Pin_8
 #define BSP_COM7_TX_GPIO_PORT               GPIOE
 #define BSP_COM7_TX_GPIO_CLK                RCC_AHB1Periph_GPIOE
 #define BSP_COM7_RX_AF_PIN                  GPIO_PinSource7
 #define BSP_COM7_RX_PIN                     GPIO_Pin_7
 #define BSP_COM7_RX_GPIO_PORT               GPIOE
 #define BSP_COM7_RX_GPIO_CLK                RCC_AHB1Periph_GPIOE
 #endif
         
 #define BSP_COM7_IRQn                       UART7_IRQn
         
 #define BSP_COM7_DMA_DR_BASE                0x40007804
 #define BSP_COM7_DMA_CLK                    RCC_AHB1Periph_DMA1
 #define BSP_COM7_Tx_DMA_FLAG                DMA_FLAG_TCIF1
 #define BSP_COM7_Tx_DMA_Stream              DMA1_Stream1
 #define BSP_COM7_Tx_DMA_Channel             DMA_Channel_5
 
 #define BSP_COM7_Tx_BUFF_SIZE               0x400       //发送缓冲区大小
 #define BSP_COM7_Rx_BUFF_SIZE               0x400       //接收缓冲区大小
 
 /**
  * @brief Definition for COM port8, connected to USART8
  */ 
 #define BSP_COM8                            UART8
 #define BSP_COM8_CLK                        RCC_APB1Periph_UART8
 #define BSP_COM8_AF                         GPIO_AF_UART8
 
 #define BSP_COM8_TX_AF_PIN                  GPIO_PinSource1
 #define BSP_COM8_TX_PIN                     GPIO_Pin_1
 #define BSP_COM8_TX_GPIO_PORT               GPIOE
 #define BSP_COM8_TX_GPIO_CLK                RCC_AHB1Periph_GPIOE
 #define BSP_COM8_RX_AF_PIN                  GPIO_PinSource0
 #define BSP_COM8_RX_PIN                     GPIO_Pin_0
 #define BSP_COM8_RX_GPIO_PORT               GPIOE
 #define BSP_COM8_RX_GPIO_CLK                RCC_AHB1Periph_GPIOE
         
 #define BSP_COM8_IRQn                       UART8_IRQn
         
 #define BSP_COM8_DMA_DR_BASE                0x40007C04
 #define BSP_COM8_DMA_CLK                    RCC_AHB1Periph_DMA1
 #define BSP_COM8_Tx_DMA_FLAG                DMA_FLAG_TCIF0
 #define BSP_COM8_Tx_DMA_Stream              DMA1_Stream0
 #define BSP_COM8_Tx_DMA_Channel             DMA_Channel_5
 
 #define BSP_COM8_Tx_BUFF_SIZE               0x400       //发送缓冲区大小
 #define BSP_COM8_Rx_BUFF_SIZE               0x400       //接收缓冲区大小
 
 
 
 /******************************函数声明****************************************/
 void BSP_UartOpen(uint8_t COM,  uint32_t baud, uint8_t data, uint8_t stop, uint8_t parity);
 void BSP_UartClose(uint8_t COM);
 uint32_t BSP_UartWrite(uint8_t COM, uint8_t *buffter, uint32_t len);
 uint32_t BSP_UartRead(uint8_t COM, uint8_t *buffter, uint32_t len);
 uint32_t BSP_UartTxIdleState(uint8_t COM);
 uint32_t BSP_UartSend(uint8_t COM, uint8_t *buffter, uint32_t len);
 
 
 #endif

