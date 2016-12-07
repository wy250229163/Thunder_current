#ifndef _LCD1602_H__ 
#define _LCD1602_H__ 
#include <stm32f10x.h>  
void LCD1602_Init(void); 
void LCDwrite_com(u8 com); 
void LCDwrite_data(u8 date); 
void LCDwrite_char(u8 x,u8 y,u8 ch);
void LCDwrite_str(u8 x,u8 y,u8 *str);
void LCD_clear(void);

#endif 
