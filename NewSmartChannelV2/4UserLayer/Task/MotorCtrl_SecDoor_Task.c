/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : MotorCtrl_SecDoor_Task.c
  版 本 号   : 初稿
  作    者   :  
  生成日期   : 2020年4月28日
  最近修改   :
  功能描述   : 第二个电机控制部分
  函数列表   :
  修改历史   :
  1.日    期   : 2020年4月28日
    作    者   :  
    修改内容   : 创建文件

******************************************************************************/


/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include"MotorCtrl_SecDoor_Task.h"
#include "bsp_uart_fifo.h"
#include "comm.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
#define MOTOR_SEC_TASK_PRIO		( tskIDLE_PRIORITY + 7)
#define MOTOR_SEC_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)


/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
const char *MotorCtrlSecDoorTaskName = "vSecMotorTask";      

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMotorCtrlSecDoor = NULL;  

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static void vTaskMotorCtrlSecDoor(void *pvParameters);



void CreateMotorCtrlSecDoorTask(void)
{
    //创建电机信息返回任务
    xTaskCreate((TaskFunction_t )vTaskMotorCtrlSecDoor,     
                (const char*    )MotorCtrlSecDoorTaskName,   
                (uint16_t       )MOTOR_SEC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_SEC_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotorCtrlSecDoor);     

}

static void vTaskMotorCtrlSecDoor(void *pvParameters)
{  
    BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdPASS */

    uint8_t ReadStatus[8] = { 0x01,0x03,0x07,0x04,0x00,0x0A,0x85,0x78 };
    uint16_t crc_data = 0;
    uint8_t tmpBuf[8] = {0};
    uint8_t sendBuf[8] = {0};
    uint8_t direction = 0;
    uint8_t isEnable = ENABLE_SEND;
    uint32_t expectValue = 0;
    uint32_t currentValue = 0;
    float percentage = 0.0;

    FROMHOST_STRU rxFromHost;
    MOTORCTRL_QUEUE_T *ptMotor; 
    
	/* 初始化结构体指针 */
	ptMotor = &gSecMotorCtrlQueue;

    /* 清零 */
    ptMotor->cmd = 0;
    ptMotor->len = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
    memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));  
    
    while (1)
    {   
        memset(sendBuf,0x00,sizeof(sendBuf));

        //获取到，则执行上位机指令，获取不到，则执行状态查询
        xReturn = xQueueReceive( gxMotorSecDoorCtrlQueue,    /* 消息队列的句柄 */
                                 (void *)&ptMotor,          /*这里获取的是结构体的地址 */
                                 (TickType_t)30);          /* 设置阻塞时间 */
        if(pdTRUE == xReturn)
        {
            //消息接收成功，发送接收到的消息
            dbh("recv from host and send to MB:",(char *)ptMotor->data, ptMotor->len);
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
//            DBG("Send B door query status\r\n");
            //发送默认数据包
            memcpy(sendBuf,ReadStatus,8);
        }      

        //当从入口方向进入时，已关门角度大于50%时，触发红外，不开门，继续关闭
        if(isEnable == DISABLE_SEND && sendBuf[5] == 0x02 && sendBuf[1] == 0x06)
        {
           RS485_SendBuf(COM5, ReadStatus,8);//查询B电机状态  
        }
        else
        {
           RS485_SendBuf(COM5, sendBuf,8);
        } 
        

        vTaskDelay(60);

        if(deal_motor_Parse(COM5,&rxFromHost) != 0)
        {
            dbh("recv MB and send to host:", (char *)rxFromHost.rxBuff,rxFromHost.rxCnt);  

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
                send_to_host(CONTROLMOTOR_B,tmpBuf,7);
            }
            else
            {
                send_to_host(CONTROLMOTOR_B,rxFromHost.rxBuff,rxFromHost.rxCnt);
            } 

            //direction = 0x55 表示进方向，=0xAA 表示出方向
            if(direction == 0x55 && rxFromHost.rxCnt== 25) 
            {            
                currentValue = u8ToU32(rxFromHost.rxBuff[3],rxFromHost.rxBuff[4],rxFromHost.rxBuff[5],rxFromHost.rxBuff[6]);
                expectValue = u8ToU32(rxFromHost.rxBuff[7],rxFromHost.rxBuff[8],rxFromHost.rxBuff[9],rxFromHost.rxBuff[10]);

                percentage = currentValue/expectValue;

                DBG("currentValue = %d, expectValue = %d,percentage = %f\r\n",currentValue,expectValue,percentage);
                
                //根据值设置标志位
                if(percentage > 0.5f)
                {
                    isEnable = DISABLE_SEND;
                }
                else
                {
                    isEnable = ENABLE_SEND;
                }
            }
//            else
//            {
//                isEnable = ENABLE_SEND;
//            }

            //到位后，重置状态
            if(rxFromHost.rxBuff[19] == 0x08)
            {
                direction = 0;
                isEnable = ENABLE_SEND;
            }            
            
            Motro_B = 0;
            memset(&rxFromHost,0x00,sizeof(FROMHOST_STRU));
        }
        

        /* 发送事件标志，表示任务正常运行 */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
      
    }

}






