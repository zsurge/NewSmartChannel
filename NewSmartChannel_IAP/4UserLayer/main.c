#include "def.h"


//任务优先级    

#define LED_TASK_PRIO		( tskIDLE_PRIORITY + 1)
//#define KEY_TASK_PRIO		( tskIDLE_PRIORITY + 2)
#define UPDATE_TASK_PRIO	( tskIDLE_PRIORITY + 3)
//#define START_TASK_PRIO		( tskIDLE_PRIORITY + 4)


//任务堆栈大小	
//#define START_STK_SIZE 	    128  
#define LED_STK_SIZE 		256
//#define KEY_STK_SIZE 		1024
#define UPDATE_STK_SIZE 	1024





//任务句柄
//static TaskHandle_t xHandleTaskStart = NULL;
static TaskHandle_t xHandleTaskLed = NULL;
//static TaskHandle_t xHandleTaskKey = NULL;
static TaskHandle_t xHandleTaskUpdate = NULL;







//任务函数
//static void vTaskStart(void *pvParameters);
static void vTaskLed(void *pvParameters);
//static void vTaskKey(void *pvParameters);
static void vTaskUpdate(void *pvParameters);





int main(void)
{ 

	bsp_Init();
	
//	ef_set_env("WSPIFLASH",W_SPI_FLASH_OK);
//	ef_set_env("WMCUFLASH",W_MCU_FLASH_OK);
    

	//创建开始任务
    xTaskCreate((TaskFunction_t )vTaskUpdate,            //任务函数
                (const char*    )"update",               //任务名称
                (uint16_t       )UPDATE_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )UPDATE_TASK_PRIO,        //任务优先级
                (TaskHandle_t*  )&xHandleTaskUpdate);   //任务句柄        

//    xTaskCreate((TaskFunction_t )vTaskLed,            //任务函数
//                (const char*    )"vTaskLed",               //任务名称
//                (uint16_t       )LED_STK_SIZE,        //任务堆栈大小
//                (void*          )NULL,                  //传递给任务函数的参数
//                (UBaseType_t    )LED_TASK_PRIO,        //任务优先级
//                (TaskHandle_t*  )&xHandleTaskLed);   //任务句柄                     
                
    vTaskStartScheduler();          //开启任务调度
}
 
/*
*********************************************************************************************************
*	函 数 名: vTaskStart
*	功能说明: 启动任务，等待所有任务发事件标志过来。
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
//static void vTaskStart(void *pvParameters)
//{
//	EventBits_t uxBits;
//	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* 最大延迟1000ms */
//	
//	/* 
//	  开始执行启动任务主函数前使能独立看门狗。
//	  设置LSI是32分频，下面函数参数范围0-0xFFF，分别代表最小值1ms和最大值4095ms
//	  下面设置的是4s，如果4s内没有喂狗，系统复位。
//	*/
//	bsp_InitIwdg(4000);
//	
//	/* 打印系统开机状态，方便查看系统是否复位 */
//	App_Printf("=====================================================\r\n");
//	App_Printf("=系统开机执行\r\n");
//	App_Printf("=====================================================\r\n");
//	
//    while(1)
//    {
//		/* 等待所有任务发来事件标志 */
//		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
//							         TASK_BIT_ALL,       /* 等待TASK_BIT_ALL被设置 */
//							         pdTRUE,             /* 退出前TASK_BIT_ALL被清除，这里是TASK_BIT_ALL都被设置才表示“退出”*/
//							         pdTRUE,             /* 设置为pdTRUE表示等待TASK_BIT_ALL都被设置*/
//							         xTicksToWait); 	 /* 等待延迟时间 */
//		
//		if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
//		{
//            //App_Printf("七个用户任务都正常运行\r\n");
//			IWDG_Feed(); //喂狗
//			
//		}
//	    else
//		{
//			/* 基本是每xTicksToWait进来一次 */
//			/* 通过变量uxBits简单的可以在此处检测那个任务长期没有发来运行标志 */
//		}
//    }
//}


//LED1任务函数
static void vTaskLed(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
    }
}

//static void vTaskKey(void *pvParameters)
//{
//    
//	uint8_t ucKeyCode;
//	uint8_t pcWriteBuffer[500];
//    
//    while(1)
//    {
//		ucKeyCode = bsp_key_Scan(0);
//		
//		if (ucKeyCode != 0)
//		{
//			switch (ucKeyCode)
//			{
//				/* K1键按下 打印任务执行情况 */
//				case 1:			 
//					printf("=================================================\r\n");
//					printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
//					vTaskList((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);
//                    
//					printf("\r\n任务名       运行计数         使用率\r\n");
//					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);                    
//					break;				
//				/* K2键按下，打印串口操作命令 */
//				case 2:
//					//FlashTest();
//					ReadIAP();
//					break;
//				case 3:
//					ef_print_env();
//					break;
//				case 4:
//                    printf("KEY_DOWN_K4\r\n");
//                    //RestoreDefaultSetting();
//                    IAP_DownLoadToFlash();
//					break;                
//				
//				/* 其他的键值不处理 */
//				default:   
//				printf("KEY_default\r\n");
//					break;
//			}
//		}
//		
//		vTaskDelay(20);
//	}   

//}


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
        printf("spi_flash_value = %s,spi_flash_flag = %d\r\n",spi_flash_value,spi_flash_flag);

        mcu_flash_value = ef_get_env("WMCUFLASH"); 
        mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flash_value);       
        printf("mcu_flash_value = %s,mcu_flash_flag = %d\r\n",mcu_flash_value,mcu_flash_flag);
        
        //正常跳转
        if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
        {
            //不需要升级，进入APP
			printf("APP STATR!\r\n");
//            IAP_JumpToApplication();
			if(IAP_JumpToApplication())
			{
                printf("iap jump error,please download app\r\n");

                //跳转失败，所有要重置所有标志位，升级写SPI FLASH和MCU FLASH
                ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);                
            }            
        }
        else
        {
			printf("IAP STATR! \r\n");

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





