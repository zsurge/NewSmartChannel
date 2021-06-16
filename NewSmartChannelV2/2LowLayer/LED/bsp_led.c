#include "bsp_led.h" 
#include "string.h"
#include "tool.h"
 

//��ʼ��PF9��PF10Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void bsp_LED_Init(void)
{    	 
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_AHB1PeriphClockCmd(RCC_ALL_LED, ENABLE);//ʹ��GPIOFʱ��
    
      //GPIOF1
      GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED1 ;   //LED1��ӦIO��
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //��ͨ���ģʽ
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //����
      GPIO_Init(GPIO_PORT_LED1, &GPIO_InitStructure);         //��ʼ��GPIO
    
      //GPIOF23
      GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED2|GPIO_PIN_LED3 ; //LED2��LED3��ӦIO��
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //��ͨ���ģʽ
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //����
      GPIO_Init(GPIO_PORT_LED23, &GPIO_InitStructure);            //��ʼ��GPIO
    
      //GPIOF4
      GPIO_InitStructure.GPIO_Pin = GPIO_PIN_LED4 ;   //LED4
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //��ͨ���ģʽ
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;            //����
      GPIO_Init(GPIO_PORT_LED4, &GPIO_InitStructure);         //��ʼ��GPIO        
    
    
//      GPIO_SetBits(GPIO_PORT_LED1,GPIO_PIN_LED1 );            
//      GPIO_SetBits(GPIO_PORT_LED23,GPIO_PIN_LED2|GPIO_PIN_LED3);
//      GPIO_SetBits(GPIO_PORT_LED4,GPIO_PIN_LED4);

      LED1 = 0;
      LED2 = 0;
      LED3 = 0;
      LED4 = 0;

}


void bsp_Ex_LED_Init(void)
{    	 
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_EX_LED, ENABLE);	//ʹ��GPIOFʱ��

	//GPIOG9,D2��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_L_G|GPIO_PIN_L_R|GPIO_PIN_M_R|GPIO_PIN_M_G|GPIO_PIN_R_R|GPIO_PIN_R_G;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;		//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;		//��©��������������������ְ������������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;    
	GPIO_Init(GPIO_PORT_EX_LED, &GPIO_InitStructure);			    //��ʼ��GPIO

	//GPIO_SetBits(GPIO_PORT_EX_LED,GPIO_PIN_L_G|GPIO_PIN_M_R|GPIO_PIN_M_G|GPIO_PIN_R_R|GPIO_PIN_R_G );	//����

    LED_L_R = 1;
    LED_L_G = 1;
    LED_M_R = 1;
    LED_M_G = 1;
    LED_R_R = 1;
    LED_R_G = 1;

}


void bsp_Ex_SetLed(uint8_t *dat)
{
    uint8_t buf[16] = {0};

    if(!dat)
    {
        DBG("param error!\r\n");
        return;
    }
    
    memcpy(buf,dat,15);    

//   ȫ��    
//            r     g
//    L 4C   6C 01 67 00   
//    M 4D   6C 01 67 00
//    R 52   6C 01 67 00

    if(buf[0] == 0x4C && buf[5] == 0x4D && buf[10] == 0x52)
    {
        LED_L_R = buf[2];
        LED_L_G = buf[4];
        LED_M_R = buf[7];
        LED_M_G = buf[9];
        LED_R_R = buf[12];
        LED_R_G = buf[14];

//        DBG("2,4,7,9,12,14 = %d,%d,%d,%d,%d,%d\r\n",buf[2],buf[4],buf[7],buf[9],buf[12],buf[14]);
    }    
    
}


void bsp_speaker_init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; 

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 ;   
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;           //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;      //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);         //��ʼ��GPIO

    GPIO_SetBits(GPIOA,GPIO_Pin_1);       
}


