#include "sys.h" 
#include "usart.h"	
#include "delay.h"	
#include "LCD1602.h"  

#define rd  (1<<12)	 //PC12
#define wr  (1<<11)	 //PC11
#define en  (1<<10)	 //PC10

//设置使能脚为1或者0      
#define	lcden(x) GPIOC->ODR=(GPIOC->ODR&~en)|(x?en:0) 
#define	lcdrd(x) GPIOC->ODR=(GPIOC->ODR&~rd)|(x?rd:0) 
#define	lcdwr(x) GPIOC->ODR=(GPIOC->ODR&~wr)|(x?wr:0) 




/*********************************************************************
*名    称：LCD1602_Init()

*功    能：初始化LCD1602

*常用命令：0x38:16*2显示，5*&点阵显示字符，8位数据;指令执行时间40us

*          0x0C:开显示，关光标   40us

*          0x08:关显示，关光标   40us

*          0x0D:字符闪烁，关光标，接着设定位置，闪烁周期0.4ms左右

*          0x0F:字符闪烁，开光标

*          0x06:写完数据自动右移光标，普通情形，从左向右显示  40us

*          0x04:写完数据自动左移光标，可以从右向左显示  40us

*          0x01:清除显示内容，即清屏  1.64ms

*          0x02:使光标还回起始位置   1.64ms

*          0x18:屏幕上所有字符同时左移一格，适合滚动显示 40us

*          0x1C:屏幕上所有字符同时右移一格，适合滚动显示 40us

*********************************************************************/
  
void LCD1602_Init() 
{ 
		RCC->APB2ENR|=1<<3;        //使能PORTB时钟 
		RCC->APB2ENR|=1<<4;        //使能PORTC时钟 

		GPIOB->CRH&=0X0000FFFF;
		GPIOB->CRH|=0X33330000;  
		GPIOB->ODR&=0XFFF; 	  	
		GPIOC->CRL&=0X00FFFFFF;   
		GPIOC->CRL|=0X33000000;   
		GPIOC->ODR&=0X3FFF; 	  
		GPIOC->CRH&=0XFFF00000;   
		GPIOC->CRH|=0X00033333;   
		GPIOC->ODR&=0XFFE0; 	
	
		LCDwrite_com(0x38);  //  显示模式设置 
		delay_ms(1);	 
		LCDwrite_com(0x38);
		delay_ms(1);
		LCDwrite_com(0x38);
		delay_ms(1);
 		LCDwrite_com(0x08);  //数据指针初始化 
		delay_ms(1);
		LCDwrite_com(0x06);	//移动光标 
		delay_ms(1);
 		LCDwrite_com(0x0c);  //显示开关 及光标设置 
		delay_ms(1);
		LCDwrite_com(0x80);	 
} 



void LCDwrite_com(u8 com)	 //写指令  
{ 		
		lcden(0);	   
		lcdwr(0);
		lcdrd(0);
		GPIOB->ODR&=~0xf000;
	  GPIOC->ODR&=~0x3c0;
	  GPIOB->ODR|=(com & 0xf)<<12;
		GPIOC->ODR|=(com >> 4 )<<6;
    lcden(1);	 
	 	delay_us(50);
		lcden(0); 
} 

void LCDwrite_data(u8 data)	  //写数据  
 {  
   	lcden(0);	   
		lcdwr(0);
		lcdrd(1);
		GPIOB->ODR&=~0xf000;
	  GPIOC->ODR&=~0x3c0;
	  GPIOB->ODR|=(data & 0x0f)<<12;
		GPIOC->ODR|=(data >> 4 )<<6;
    lcden(1);	 
	 	delay_us(50);
		lcden(0);
  } 

/*********************************************************************
										    
*功  能：在指定位置显示一个字符 

*入口参数：y:行 0或1  x:列，0~15，ch：待显示的字符

*********************************************************************/
					
void LCDwrite_char(u8 x,u8 y,u8 ch)
{
       if(y==0)
              LCDwrite_com(0x80+x);
       else
              LCDwrite_com(0xC0+x);

       LCDwrite_data(ch);

}

/*********************************************************************

*名    称：LCD_write_str()

*功    能：使LCD1602显示一个字符串

*入口参数：y:行 0或1   x:列，0~15，str[]:待显示的字符串

*出口参数：无

*********************************************************************/

void LCDwrite_str(u8 x,u8 y,u8 *str)

{
       if(y==0)
              LCDwrite_com(0x80+x);

       else
              LCDwrite_com(0xC0+x);

       while(*str != '\0')
       {
              LCDwrite_data(*str);
              str++; 
       }
}

/******************************************************************/

/*                        清屏函数                                */

/******************************************************************/

void LCD_clear(void)
{
       LCDwrite_com(0x01);
}



 
