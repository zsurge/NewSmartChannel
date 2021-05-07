/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : main.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��7��9��
  ����޸�   :
  ��������   : ������ģ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��7��9��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "def.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
//#define APPCREATE_TASK_PRIO		(tskIDLE_PRIORITY)
//#define APPCREATE_STK_SIZE 		(configMINIMAL_STACK_SIZE*16)
    


/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
//const char *AppCreateTaskName = "vAppCreateTask";      

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
//static TaskHandle_t xHandleTaskAppCreate = NULL;     

SemaphoreHandle_t gxMutex = NULL;
EventGroupHandle_t xCreatedEventGroup = NULL;
QueueHandle_t gxMotorCtrlQueue = NULL; 
QueueHandle_t gxMotorSecDoorCtrlQueue = NULL;
QueueHandle_t gxToHostQueue = NULL;



static void AppTaskCreate(void);
static void AppObjCreate (void);
static void AppPrintf(char *format, ...);


int main(void)
{  
    //Ӳ����ʼ��
    bsp_Init();

    //��������ͨ�Ż���  
    AppObjCreate();  

    //����AppTaskCreate����
    AppTaskCreate();
//    xTaskCreate((TaskFunction_t )AppTaskCreate,     
//                (const char*    )AppCreateTaskName,   
//                (uint16_t       )APPCREATE_STK_SIZE, 
//                (void*          )NULL,
//                (UBaseType_t    )APPCREATE_TASK_PRIO,
//                (TaskHandle_t*  )&xHandleTaskAppCreate);   

    
    /* �������ȣ���ʼִ������ */
    vTaskStartScheduler();
    
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    //�����ٽ���
//    taskENTER_CRITICAL(); 
    
    //����
//    CreateHandShakeTask();

    //����ָʾ��
    CreateLedTask();//5     0

    //����λ��ͨѶ����
    CreateMsgParseTask();//8 2
    
    //A�ŵ�����ƴ���
    CreateMotorCtrlTask();//6 1

    //B�ŵ�����ƴ���
    CreateMotorCtrlSecDoorTask(); //7 6

    //��������
//    CreateToHostTask();

    //��������
    CreateKeyTask();//4     7

    //�����������ռ�
    CreateReaderTask();//3  4

    //����ɨ�����ݴ���
    CreateBarCodeTask();//2     5

    //���⴫������������
    CreateSensorTask();//1      3

    //���Ź�
    CreateWatchDogTask();//9

    //ɾ������
//    vTaskDelete(xHandleTaskAppCreate); //ɾ��AppTaskCreate����

    //�˳��ٽ���
//    taskEXIT_CRITICAL();            

}


/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
//    /* ����QueuSet */
//    gxQueueSet = xQueueCreateSet(COMBINED_LENGTH);
//	
//	if(gxQueueSet == NULL)
//    {
//        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
//         AppPrintf("����QueuSetʧ��\r\n");
//    }


	/* �����¼���־�� */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
        AppPrintf("�����¼���־��ʧ��\r\n");
    }

	/* ���������ź��� */
    gxMutex = xSemaphoreCreateMutex();
	
	if(gxMutex == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
        AppPrintf("���������ź���ʧ��\r\n");
    }     
    
  
    /* ������Ϣ���� */
    gxMotorCtrlQueue = xQueueCreate((UBaseType_t ) MOTORCTRL_QUEUE_LEN,/* ��Ϣ���еĳ��� */
                              (UBaseType_t ) sizeof(MOTORCTRL_QUEUE_T *));/* ��Ϣ�Ĵ�С */
    if(gxMotorCtrlQueue == NULL)
    {
        AppPrintf("����xTransQueue1��Ϣ����ʧ��!\r\n");
    }	

    /* ������Ϣ���� */
    gxMotorSecDoorCtrlQueue = xQueueCreate((UBaseType_t ) MOTORCTRL_QUEUE_LEN,/* ��Ϣ���еĳ��� */
                              (UBaseType_t ) sizeof(MOTORCTRL_QUEUE_T *));/* ��Ϣ�Ĵ�С */
    if(gxMotorSecDoorCtrlQueue == NULL)
    {
        AppPrintf("����xTransQueue2��Ϣ����ʧ��!\r\n");
    }	


    /* ������Ϣ���� */
    gxToHostQueue = xQueueCreate((UBaseType_t ) MOTORCTRL_QUEUE_LEN,/* ��Ϣ���еĳ��� */
                              (UBaseType_t ) sizeof(TOHOST_QUEUE_T *));/* ��Ϣ�Ĵ�С */
    if(gxToHostQueue == NULL)
    {
        AppPrintf("����gxToHostQueue��Ϣ����ʧ��!\r\n");
    }
    


    /* ��ӵ�queue setʱ����Ϣ���к��ź�������Ϊ��*/
    /* �����Ϣ���к��ź�����Queue Set */
//    #define configUSE_QUEUE_SETS 1    
//    xQueueAddToSet(gxMotorCtrlQueue, gxQueueSet);
//    xQueueAddToSet(gxMotorSecDoorCtrlQueue, gxQueueSet);
}

/*
*********************************************************************************************************
*	�� �� ��: AppPrintf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void  AppPrintf(char *format, ...)
{
    char  buf_str[512 + 1];
    va_list   v_args;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);    

	/* �����ź��� */
	xSemaphoreTake(gxMutex, portMAX_DELAY);

    printf("%s", buf_str);

   	xSemaphoreGive(gxMutex);
}








