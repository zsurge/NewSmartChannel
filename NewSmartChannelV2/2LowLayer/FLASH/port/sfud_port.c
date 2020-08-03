/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include "delay.h"

//GPIOA define
#define FLASH_PORT			GPIOB

#define FLASH_CS_PIN		GPIO_Pin_12

#define FLASH_SCK_PIN		GPIO_Pin_13
#define FLASH_MISO_PIN		GPIO_Pin_14
#define FLASH_MOSI_PIN		GPIO_Pin_15

#define FLASH_CS_PINSource		GPIO_PinSource12

#define FLASH_SCK_PINSource		GPIO_PinSource13
#define FLASH_MISO_PINSource	GPIO_PinSource14
#define FLASH_MOSI_PINSource	GPIO_PinSource15

#define FLASH_AF                GPIO_AF_SPI2

/* Ƭѡ�����õ�ѡ��  */
#define SF_CS_LOW()       FLASH_PORT->BSRRH = FLASH_CS_PIN

/* Ƭѡ�����ø߲�ѡ�� */
#define SF_CS_HIGH()      FLASH_PORT->BSRRL = FLASH_CS_PIN



typedef struct {
    SPI_TypeDef *spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
} spi_user_data, *spi_user_data_t;


static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

static void rcc_configuration(spi_user_data_t spi) {
    if (spi->spix == SPI1) {
         /* you can add SPI1code here */   

    } else if (spi->spix == SPI2) {
        /* you can add SPI2 code here */    
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ��SPI2ʱ��
    }
}

static void gpio_configuration(spi_user_data_t spi) {
    GPIO_InitTypeDef GPIO_InitStructure;

    if (spi->spix == SPI1) {    

    } else if (spi->spix == SPI2) {
        /* you can add SPI2 code here */
    	GPIO_PinAFConfig(FLASH_PORT,FLASH_SCK_PINSource,FLASH_AF); //PB3����Ϊ SPI2
    	GPIO_PinAFConfig(FLASH_PORT,FLASH_MISO_PINSource,FLASH_AF); //PB4����Ϊ SPI2
    	GPIO_PinAFConfig(FLASH_PORT,FLASH_MOSI_PINSource,FLASH_AF); //PB5����Ϊ SPI2
    	
        /* SCK:PB3  MISO:PB4  MOSI:PB5 */
        GPIO_InitStructure.GPIO_Pin = FLASH_SCK_PIN|FLASH_MISO_PIN|FLASH_MOSI_PIN;//PB3~5���ù������  
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
        GPIO_Init(FLASH_PORT, &GPIO_InitStructure);//��ʼ��  

        SF_CS_HIGH();
        //GPIOB14
        GPIO_InitStructure.GPIO_Pin = FLASH_CS_PIN;//PB14
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
        GPIO_Init(FLASH_PORT, &GPIO_InitStructure);//��ʼ��      
    }
}

static void spi_configuration(spi_user_data_t spi) {
    SPI_InitTypeDef SPI_InitStructure;	

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI ����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      //����Ϊ�� SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI ���ͽ��� 8 λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                         //ʱ�����յ�
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                       //���ݲ����ڵ�һ��ʱ����
    //TODO �Ժ���Գ���Ӳ�� CS
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          //�ڲ�  NSS �ź��� SSI λ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; //������Ԥ��ƵֵΪ 4
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 //���ݴ���� MSB λ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;                           // CRC ֵ����Ķ���ʽ

    SPI_I2S_DeInit(spi->spix);
    SPI_Init(spi->spix, &SPI_InitStructure);

    SPI_CalculateCRC(spi->spix, DISABLE);
    SPI_Cmd(spi->spix, ENABLE);
}

static void spi_lock(const sfud_spi *spi) {
    __disable_irq();
}

static void spi_unlock(const sfud_spi *spi) {
    __enable_irq();
}





/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
        size_t read_size) 
{
    sfud_err result = SFUD_SUCCESS;
    uint8_t send_data, read_data;
    
    spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

    GPIO_ResetBits(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);
    /* ��ʼ��д���� */
    for (size_t i = 0, retry_times; i < write_size + read_size; i++) {
        /* ��д�������е����ݵ� SPI ���ߣ�����д�����д dummy(0xFF) �� SPI ���� */
        if (i < write_size) {
            send_data = *write_buf++;
        } else {
            send_data = SFUD_DUMMY_DATA;
        }
        /* �������� */
        retry_times = 1000;
        while (SPI_I2S_GetFlagStatus(spi_dev->spix, SPI_I2S_FLAG_TXE) == RESET) {
            SFUD_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != SFUD_SUCCESS) {
            goto exit;
        }
        SPI_I2S_SendData(spi_dev->spix, send_data);
        /* �������� */
        retry_times = 1000;
        while (SPI_I2S_GetFlagStatus(spi_dev->spix, SPI_I2S_FLAG_RXNE) == RESET) {
            SFUD_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != SFUD_SUCCESS) {
            goto exit;
        }
        read_data = SPI_I2S_ReceiveData(spi_dev->spix);
        /* д�������е����ݷ�����ٶ�ȡ SPI �����е����ݵ��������� */
        if (i >= write_size) {
            *read_buf++ = read_data;
        }
    }

exit:
    GPIO_SetBits(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);

    return result;

}


static spi_user_data spi1 = { .spix = SPI2, .cs_gpiox = GPIOB, .cs_gpio_pin = GPIO_Pin_12 };


#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
        uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */

    return result;
}
#endif /* SFUD_USING_QSPI */

sfud_err sfud_spi_port_init(sfud_flash *flash) 
{
    sfud_err result = SFUD_SUCCESS;
    
    /* RCC ��ʼ�� */
    rcc_configuration(&spi1);
    /* GPIO ��ʼ�� */
    gpio_configuration(&spi1);
    /* SPI �����ʼ�� */
    spi_configuration(&spi1);
    
    /* ͬ�� Flash ��ֲ����Ľӿڼ����� */
    flash->spi.wr = spi_write_read;
    flash->spi.lock = spi_lock;
    flash->spi.unlock = spi_unlock;
    flash->spi.user_data = &spi1;
    /* about 100 microsecond delay */
    flash->retry.delay = retry_delay_100us;
    /* adout 60 seconds timeout */
    flash->retry.times = 60 * 10000;

    return result;

}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\n", log_buf);
    va_end(args);
}