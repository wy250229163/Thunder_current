#include "exti.h"
#include "delay.h"
#include "usart.h"
#include "LCD1602.h"
#include "adc.h"
#include "data.h"

//外部中断初始化程序
void EXTI_GETDATA_Init(void)
{
	RCC->APB2ENR|=1<<3;     //使能PORTB时钟
	GPIOB->CRL&=0XFFFFFF00;	//PB0/1设置成输入
	GPIOB->CRL|=0X00000088; 	
	GPIOB->ODR|=3<<0; //PB0/1上拉
	
	Ex_NVIC_Config(GPIO_B,0,RTIR);
	Ex_NVIC_Config(GPIO_B,1,RTIR);
	
	MY_NVIC_Init(2,1,EXTI0_IRQn,2);	//抢占0，子优先级0，组2
	MY_NVIC_Init(2,0,EXTI1_IRQn,2);	//抢占0，子优先级1，组2
}

