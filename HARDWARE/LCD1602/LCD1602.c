#include "sys.h" 
#include "usart.h"	
#include "delay.h"	
#include "LCD1602.h"  

#define rd  (1<<12)	 //PC12
#define wr  (1<<11)	 //PC11
#define en  (1<<10)	 //PC10

//����ʹ�ܽ�Ϊ1����0      
#define	lcden(x) GPIOC->ODR=(GPIOC->ODR&~en)|(x?en:0) 
#define	lcdrd(x) GPIOC->ODR=(GPIOC->ODR&~rd)|(x?rd:0) 
#define	lcdwr(x) GPIOC->ODR=(GPIOC->ODR&~wr)|(x?wr:0) 




/*********************************************************************
*��    �ƣ�LCD1602_Init()

*��    �ܣ���ʼ��LCD1602

*�������0x38:16*2��ʾ��5*&������ʾ�ַ���8λ����;ָ��ִ��ʱ��40us

*          0x0C:����ʾ���ع��   40us

*          0x08:����ʾ���ع��   40us

*          0x0D:�ַ���˸���ع�꣬�����趨λ�ã���˸����0.4ms����

*          0x0F:�ַ���˸�������

*          0x06:д�������Զ����ƹ�꣬��ͨ���Σ�����������ʾ  40us

*          0x04:д�������Զ����ƹ�꣬���Դ���������ʾ  40us

*          0x01:�����ʾ���ݣ�������  1.64ms

*          0x02:ʹ��껹����ʼλ��   1.64ms

*          0x18:��Ļ�������ַ�ͬʱ����һ���ʺϹ�����ʾ 40us

*          0x1C:��Ļ�������ַ�ͬʱ����һ���ʺϹ�����ʾ 40us

*********************************************************************/
  
void LCD1602_Init() 
{ 
		RCC->APB2ENR|=1<<3;        //ʹ��PORTBʱ�� 
		RCC->APB2ENR|=1<<4;        //ʹ��PORTCʱ�� 

		GPIOB->CRH&=0X0000FFFF;
		GPIOB->CRH|=0X33330000;  
		GPIOB->ODR&=0XFFF; 	  	
		GPIOC->CRL&=0X00FFFFFF;   
		GPIOC->CRL|=0X33000000;   
		GPIOC->ODR&=0X3FFF; 	  
		GPIOC->CRH&=0XFFF00000;   
		GPIOC->CRH|=0X00033333;   
		GPIOC->ODR&=0XFFE0; 	
	
		LCDwrite_com(0x38);  //  ��ʾģʽ���� 
		delay_ms(1);	 
		LCDwrite_com(0x38);
		delay_ms(1);
		LCDwrite_com(0x38);
		delay_ms(1);
 		LCDwrite_com(0x08);  //����ָ���ʼ�� 
		delay_ms(1);
		LCDwrite_com(0x06);	//�ƶ���� 
		delay_ms(1);
 		LCDwrite_com(0x0c);  //��ʾ���� ��������� 
		delay_ms(1);
		LCDwrite_com(0x80);	 
} 



void LCDwrite_com(u8 com)	 //дָ��  
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

void LCDwrite_data(u8 data)	  //д����  
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
										    
*��  �ܣ���ָ��λ����ʾһ���ַ� 

*��ڲ�����y:�� 0��1  x:�У�0~15��ch������ʾ���ַ�

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

*��    �ƣ�LCD_write_str()

*��    �ܣ�ʹLCD1602��ʾһ���ַ���

*��ڲ�����y:�� 0��1   x:�У�0~15��str[]:����ʾ���ַ���

*���ڲ�������

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

/*                        ��������                                */

/******************************************************************/

void LCD_clear(void)
{
       LCDwrite_com(0x01);
}



 
