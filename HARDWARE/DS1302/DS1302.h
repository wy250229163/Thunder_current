#include <stm32f10x.h>

void DS1302_Init(void);
void DS1302UpdateTime(u8 *data);//�������ں���
void DS1302ReadTime(u8 *buffer);//��ȡоƬ���ʱ��ֵ
