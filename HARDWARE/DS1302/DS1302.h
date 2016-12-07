#include <stm32f10x.h>

void DS1302_Init(void);
void DS1302UpdateTime(u8 *data);//设置日期函数
void DS1302ReadTime(u8 *buffer);//读取芯片里的时间值
