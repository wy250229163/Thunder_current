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
	RCC->APB2ENR|=1<<4;     //使能PORTC时钟
	GPIOB->CRL&=0XFFFFFF00;	//PB0/1设置成输入
	GPIOB->CRL|=0X00000088; 	
	GPIOB->ODR|=3<<0; //PB0/1上拉
	GPIOC->CRL&=0XFFFF0FFF;	//PC3推挽输出
	GPIOC->CRL|=0X00003000; 	
	SIG_RST=0; //初始化PC3为0
	
	
	Ex_NVIC_Config(GPIO_B,0,FTIR);
	Ex_NVIC_Config(GPIO_B,1,FTIR);
	
	MY_NVIC_Init(2,1,EXTI0_IRQn,2);	//抢占0，子优先级0，组2
	MY_NVIC_Init(2,0,EXTI1_IRQn,2);	//抢占0，子优先级1，组2
}

