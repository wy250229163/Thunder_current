#include "exti.h"
#include "delay.h"
#include "usart.h"
#include "LCD1602.h"
#include "adc.h"
#include "data.h"

//�ⲿ�жϳ�ʼ������
void EXTI_GETDATA_Init(void)
{
	RCC->APB2ENR|=1<<3;     //ʹ��PORTBʱ��
	GPIOB->CRL&=0XFFFFFF00;	//PB0/1���ó�����
	GPIOB->CRL|=0X00000088; 	
	GPIOB->ODR|=3<<0; //PB0/1����
	
	Ex_NVIC_Config(GPIO_B,0,RTIR);
	Ex_NVIC_Config(GPIO_B,1,RTIR);
	
	MY_NVIC_Init(2,1,EXTI0_IRQn,2);	//��ռ0�������ȼ�0����2
	MY_NVIC_Init(2,0,EXTI1_IRQn,2);	//��ռ0�������ȼ�1����2
}

