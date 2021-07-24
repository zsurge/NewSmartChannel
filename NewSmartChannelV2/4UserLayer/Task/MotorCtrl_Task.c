/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : MotorCtrl_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年2月26日
  最近修改   :
  功能描述   : 电机控制任务
  函数列表   :
  修改历史   :
  1.日    期   : 2020年2月26日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include"MotorCtrl_Task.h"
#include "bsp_uart_fifo.h"
#include "comm.h"
#include "string.h"
#include "math.h"
//#include"Monitor_Task.h"
#include "bsp_led.h"


/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 6)
#define MOTOR_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)



/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *MotorCtrlTaskName = "vMotorCtrlTask";      

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMotorCtrl = NULL;     




/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskMotorCtrl(void *pvParameters);



void CreateMotorCtrlTask(void)
{
    //创建电机信息返回任务
    xTaskCreate((TaskFunction_t )vTaskMotorCtrl,     
                (const char*    )MotorCtrlTaskName,   
                (uint16_t       )MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotorCtrl);     

}

static void vTaskMotorCtrl(void *pvParameters)
{  
    BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */

    uint8_t OpenDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 };
    uint8_t ReadStatus[8] = { 0x01,0x03,0x07,0x04,0x00,0x0A,0x85,0x78 };
    uint16_t crc_data = 0;
    uint8_t tmpBuf[8] = {0};
    uint8_t sendBuf[8] = {0};
    uint8_t direction = 0;
    uint8_t isEnable = ENABLE_SEND;
    uint32_t expectValue = 0;
    uint32_t currentValue = 0;
    float percentage = 0.0;


	uint32_t tmp_expectValue = 0;
    uint32_t tmp_currentValue = 0;
    
    FROMHOST_STRU rxFromHost;  
    MOTORCTRL_QUEUE_T *ptMotor; 
	/* 初始化结构体指针 */
	ptMotor = &gMotorCtrlQueue;
    
    /* 清零 */
    ptMotor->cmd = 0;
    ptMotor->len = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    { 
       memset(sendBuf,0x00,sizeof(sendBuf));
                
        //获取到，则执行上位机指令，获取不到，则执行状态查询
        xReturn = xQueueReceive( gxMotorCtrlQueue,    /* 消息队列的句柄 */
                                 (void *)&ptMotor,  /*这里获取的是结构体的地址 */
                                 (TickType_t)30); /* 设置阻塞时间 */
        if(pdTRUE == xReturn)
        {
            //消息接收成功，发送接收到的消息
            //dbh("recv from host and send to MA:",(char *)ptMotor->data, ptMotor->len);
            if(ptMotor->len >= 8)
            {
                direction = ptMotor->data[8];
                memcpy(sendBuf,ptMotor->data,8);
            }
            else
            {
                //direction = 0;
                memcpy(sendBuf,ptMotor->data,8);
            }
        }
        else
        {
            //发送默认数据包
            //RS485_SendBuf(COM4, ReadStatus,8);//查询A电机状态
            memcpy(sendBuf,ReadStatus,8);
        }

        //当从入口方向进入时，已关门角度大于50%时，触发红外，不开门，继续关闭
        if(isEnable == DISABLE_SEND && sendBuf[5] == 0x02 && sendBuf[1] == 0x06)
        {
		   send_to_host(CONTROLMOTOR_A,OpenDoor,8); 
           //RS485_SendBuf(COM4, ReadStatus,8);//查询A电机状态  
        }
        else
        {
           RS485_SendBuf(COM4, sendBuf,8);
        }          

		
		//dbh("send to MA:",(char *)sendBuf, 8);
       
        vTaskDelay(50);

        if(deal_motor_Parse(COM4,&rxFromHost) != 0)
        { 
            //dbh("recv MA and send to host:", (char*)rxFromHost.rxBuff,rxFromHost.rxCnt);	

			
			 tmp_currentValue = u8ToU32(rxFromHost.rxBuff[3],rxFromHost.rxBuff[4],rxFromHost.rxBuff[5],rxFromHost.rxBuff[6]);
			 tmp_expectValue = u8ToU32(rxFromHost.rxBuff[7],rxFromHost.rxBuff[8],rxFromHost.rxBuff[9],rxFromHost.rxBuff[10]);

			 DBG("currentValue = <%d>, expectValue = <%d>\r\n",tmp_currentValue,tmp_expectValue);
			
            
            if(rxFromHost.rxBuff[1] == 0x03 && rxFromHost.rxCnt== 25)
            {            
                //因为查询指令改了，所以这里重新打包数据给上位机，避免上位机改动
                memset(tmpBuf,0x00,sizeof(tmpBuf));
                tmpBuf[0] = 0x01;
                tmpBuf[1] = 0x03;
                tmpBuf[2] = 0x02;
                tmpBuf[3] = rxFromHost.rxBuff[19];
                tmpBuf[4] = rxFromHost.rxBuff[20];
                crc_data = CRC16_Modbus(tmpBuf, 5);                
                tmpBuf[5] = crc_data>>8;
                tmpBuf[6] = crc_data & 0xFF;                 
                send_to_host(CONTROLMOTOR_A,tmpBuf,7);
            }
            else
            {
                send_to_host(CONTROLMOTOR_A,rxFromHost.rxBuff,rxFromHost.rxCnt);
            }          
            
            
            //direction = 0x55 表示进方向，=0xAA 表示出方向
            if(direction == 0x55 && rxFromHost.rxCnt== 25) 
            {            
                currentValue = u8ToU32(rxFromHost.rxBuff[3],rxFromHost.rxBuff[4],rxFromHost.rxBuff[5],rxFromHost.rxBuff[6]);
                expectValue = u8ToU32(rxFromHost.rxBuff[7],rxFromHost.rxBuff[8],rxFromHost.rxBuff[9],rxFromHost.rxBuff[10]);

				currentValue = abs(currentValue);
				expectValue = abs(expectValue);
				
				
				if(expectValue == 0 && percentage == 0.0F)
				{
					percentage = currentValue *0.6F;
 				}				


                DBG("currentValue = %d, expectValue = %d,percentage = %f\r\n",currentValue,expectValue,percentage);
                
                //根据值设置标志位
                if(currentValue <percentage)
                {
                    isEnable = DISABLE_SEND;
                }
                else 
                {
                    isEnable = ENABLE_SEND;
                }
            }


            //到位后，重置状态
            if(rxFromHost.rxBuff[19] == 0x08)
            {
                direction = 0;
                isEnable = ENABLE_SEND;
				expectValue = 0.0F;
            }
            
            Motro_A = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));            
        }

        /* 发送事件标志，表示任务正常运行 */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);      
    }

}



