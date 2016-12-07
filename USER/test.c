#include "sys.h"
#include "usart.h"		
#include "delay.h"	 
#include "LCD1602.h"
#include "DS1302.h"
#include "spi.h"
#include "flash.h"
#include "key.h"
#include "data.h"
#include "timer.h"
#include "adc.h"
#include "exti.h"

int main(void)
{	
	Stm32_Clock_Init(9);	//系统时钟设置
	delay_init(72);	  		//延时初始化
	TIM2_Init(4999,7199);	
	DS1302_Init();
	LCD1602_Init();
	SPI_Flash_Init();
	KEY_Init();
	ADC_Init();
	EXTI_GETDATA_Init();
	SAVEDATA_SYSTEM_Init();

  while(1)
	{
		CHEAK_ISnewdisplay();
//		tttt=GET_INchip_MAXdata(15);
//		LCDwrite_char(3,0,0x30+(u8)(tttt/1000));
//		LCDwrite_char(4,0,0x30+(u8)(tttt%1000/100));
//  	LCDwrite_char(5,0,0x30+(u8)(tttt%100/10));
//		LCDwrite_char(6,0,0x30+(u8)(tttt%10));
	}
}
