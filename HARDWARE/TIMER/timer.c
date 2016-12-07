#include "timer.h"

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时2
void TIM2_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<0;	//TIM2时钟使能    
 	TIM2->ARR=arr;  	//设定计数器自动重装值//刚好1ms    
	TIM2->PSC=psc;  	//预分频器7200,得到10Khz的计数时钟		  
	TIM2->DIER|=1<<0;   //允许更新中断	  
	TIM2->CR1|=0x01;    //使能定时器3
  MY_NVIC_Init(1,1,TIM2_IRQn,2);//抢占1，子优先级3，组2									 
}










