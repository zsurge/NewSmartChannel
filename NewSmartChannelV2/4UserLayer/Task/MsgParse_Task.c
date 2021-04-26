/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : MsgParse_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年2月27日
  最近修改   :
  功能描述   : 解析上位机指令
  函数列表   :
  修改历史   :
  1.日    期   : 2020年2月27日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "MsgParse_Task.h"
#include "comm.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define MSGPARSE_TASK_PRIO		( tskIDLE_PRIORITY + 8)
#define MSGPARSE_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *MsgParseTaskName = "vMsgParseTask";  

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMsgParse = NULL;

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskMsgParse(void *pvParameters);

void CreateMsgParseTask(void)
{
    //跟android通讯串口数据解析
    xTaskCreate((TaskFunction_t )vTaskMsgParse,     
                (const char*    )MsgParseTaskName,   
                (uint16_t       )MSGPARSE_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MSGPARSE_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMsgParse);
}



static void vTaskMsgParse(void *pvParameters)
{
    while(1)
    {          
        deal_Serial_Parse();    
	    deal_rx_data();

		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_2);
        vTaskDelay(50);
    }

}

