#include "def.h"


//任务优先级  
#define UPDATE_TASK_PRIO	( tskIDLE_PRIORITY + 3)

//任务堆栈大小	

#define LED_STK_SIZE 		256
#define UPDATE_STK_SIZE 	1024*2

//任务句柄
static TaskHandle_t xHandleTaskUpdate = NULL;

//任务函数
static void vTaskUpdate(void *pvParameters);





int main(void)
{ 

	bsp_Init();
	
    

	//创建开始任务
    xTaskCreate((TaskFunction_t )vTaskUpdate,            //任务函数
                (const char*    )"update",               //任务名称
                (uint16_t       )UPDATE_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )UPDATE_TASK_PRIO,        //任务优先级
                (TaskHandle_t*  )&xHandleTaskUpdate);   //任务句柄        
                
    vTaskStartScheduler();          //开启任务调度
}
 


static void vTaskUpdate(void *pvParameters)
{
    uint32_t file_total_size = 0;
    char *spi_flash_value;
    char *mcu_flash_value;
    char *file_size;

    int8_t spi_flash_flag = 0;
    int8_t mcu_flash_flag = 0;
    
    while(1)
    {   

        //打印下参数，DEBUG时使用，RELEASE里需删除
        //ef_print_env();

        //读取升级标志位
        spi_flash_value = ef_get_env("WSPIFLASH");
        spi_flash_flag = strcmp(W_SPI_FLASH_OK, spi_flash_value);

        mcu_flash_value = ef_get_env("WMCUFLASH"); 
        mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flash_value);       
        
        //正常跳转
        if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
        {
            //不需要升级，进入APP
			if(IAP_JumpToApplication())
			{
                printf("iap jump error,please download app\r\n");

                //跳转失败，所有要重置所有标志位，升级写SPI FLASH和MCU FLASH
                ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);  

                //add 2019.09.05测试发现，需要重启才能升级成功
                NVIC_SystemReset();
            }            
        }
        else
        {
            //判断SPI FLASH是否已写完，MCU FLASH未写完，读FLASH，写MCU FLASH
            if(spi_flash_flag == 0) //程序已存在flash内部，但是未写到mcu
            {
                printf("IAP STATR! ---> Write MCU FLASH\r\n");
                //获取文件大小
                file_size = ef_get_env((const char * )"FileSize");
                file_total_size = str2int((const char *)file_size);

                if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                {
                    //写入MCU FLASH 完成标志位
                    if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                    {
                        printf("STM_FLASH_Write success\r\n");
                    } 
                }
            }
            else //SPI FLASH无程序文件，重新下载程序文件及写入到MCU FLASH中
            {     
                printf("IAP STATR! ---> Write SPI FLASH\r\n");
                
                //需要升级，进入IAP升级流程
                file_total_size = IAP_DownLoadToFlash();
                
                if(file_total_size > 0)
                {
                    printf("write stm flash\r\n");
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //写入MCU FLASH 完成标志位
                        if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                        {
                            printf("STM_FLASH_Write success\r\n");
                        }  
                    }
                    else
                    {
                        ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);                        
                    }
                }
            }
        }        

        vTaskDelay(100);
    }
}





