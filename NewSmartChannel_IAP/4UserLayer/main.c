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
const uint8_t SoftBulidDate[] = __DATE__ " " __TIME__;
const uint8_t Softversion[] = "1.0.1";
const uint8_t DevModel[] = "SmartChannelIAP";

static void DisplayDevInfo (void);

static void DisplayDevInfo(void)
{
	printf("Softversion :%s\r\n",Softversion);
	printf("Model :%s\r\n", DevModel);
	printf("ProductBatch :%s\r\n", SoftBulidDate);	
}

int main(void)
{ 
	bsp_Init();
	
    DisplayDevInfo();

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
    char spi_flash_value[4] = {0};
    char mcu_flash_value[4] = {0};    
    char file_size_value[16] = {0};

    char spi_flash_flag = 0;
    char mcu_flash_flag = 0;
    
    char spi_flash_len = 0;
    char mcu_flash_len = 0;
    int fileLen = 0;

    
    while(1)
    {   

        //打印下参数，DEBUG时使用，RELEASE里需删除
        //ef_print_env();
        
//        ef_set_env("WSPIFLASH", W_SPI_FLASH_OK);
//        ef_set_env("WMCUFLASH", W_MCU_FLASH_OK);

//        ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);
//        while(1);

        //读取升级标志位
        spi_flash_len = ef_get_env_blob("WSPIFLASH", spi_flash_value, sizeof(spi_flash_value) , NULL);
        spi_flash_flag = memcmp(W_SPI_FLASH_NEED, spi_flash_value,spi_flash_len);

        
        mcu_flash_len = ef_get_env_blob("WMCUFLASH", mcu_flash_value, sizeof(mcu_flash_value) , NULL);
        mcu_flash_flag = memcmp(W_MCU_FLASH_NEED, mcu_flash_value,mcu_flash_len);       

        //DBG("spi value = %s,len = %d,spiflag = %d,mcu value = %s,len = %d,mcuflag = %d\r\n",\
        spi_flash_value,spi_flash_len,spi_flash_flag,mcu_flash_value,mcu_flash_len,mcu_flash_flag);

        
        //正常跳转
        if((spi_flash_flag != 0) && (mcu_flash_flag != 0))
        {
            DBG("jump app \r\n");
            //不需要升级，进入APP
			if(IAP_JumpToApplication())
			{
                DBG("iap jump error,please download app\r\n");

                //跳转失败，所有要重置所有标志位，升级写SPI FLASH和MCU FLASH
                ef_set_env_blob("WSPIFLASH", W_SPI_FLASH_NEED,4);
                ef_set_env_blob("WMCUFLASH", W_MCU_FLASH_NEED,4);  

                //add 2019.09.05测试发现，需要重启才能升级成功
                NVIC_SystemReset();
            }            
        }
        else
        {
            //判断SPI FLASH是否已写完，MCU FLASH未写完，读FLASH，写MCU FLASH
            if(spi_flash_flag == 0) //程序已存在flash内部，但是未写到mcu
            {
                DBG("IAP STATR! ---> Write SPI FLASH\r\n");
                            
                //需要升级，进入IAP升级流程
                file_total_size = IAP_DownLoadToFlash();
                
                if(file_total_size > 0)
                {
                    DBG("write stm flash\r\n");
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //写入MCU FLASH 完成标志位
                        if(ef_set_env_blob("WMCUFLASH",W_MCU_FLASH_OK,4) == EF_NO_ERR)
                        {
                            DBG("STM_FLASH_Write success\r\n");
                        }  
                    }
                    else
                    {
                        ef_set_env_blob("WMCUFLASH", W_MCU_FLASH_NEED,4);                        
                    }
                }
            }
            else //SPI FLASH无程序文件，重新下载程序文件及写入到MCU FLASH中
            {  

                DBG("IAP STATR! ---> Write MCU FLASH\r\n");
                //获取文件大小
                //file_size = ef_get_env((const char * )"FileSize");
                
                ef_get_env_blob("FileSize", NULL, 0, &fileLen);
                fileLen = ef_get_env_blob("FileSize", file_size_value, sizeof(file_size_value) , NULL);  

                file_total_size = str2int((const char *)file_size_value);

                if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                {
                    //写入MCU FLASH 完成标志位
                    if(ef_set_env_blob("WMCUFLASH",W_MCU_FLASH_OK,4) == EF_NO_ERR)
                    {
                        DBG("STM_FLASH_Write success\r\n");
                    } 
                }                
            }
        }        

        vTaskDelay(100);
    }
}





