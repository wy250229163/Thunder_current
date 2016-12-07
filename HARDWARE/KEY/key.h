#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "LCD1602.h"	 

#define KEY1_PRES	1		//KEY1按下
#define KEY2_PRES	2		//KEY2按下
#define KEY3_PRES	3		//KEY3按下
#define KEY4_PRES	4		//KEY4按下

#define KEY1  PAin(0)  
#define KEY2  PCin(0)	 	 
#define KEY3  PCin(1)	 	
#define KEY4  PCin(2)
	 
void KEY_Init(void);		//IO初始化
u8 KEY_Scan(u8 mode);		//按键扫描函数
void CHEAK_ISnewdisplay(void);



#endif
