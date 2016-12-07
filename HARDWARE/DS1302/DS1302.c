#include "delay.h"
#include "DS1302.h"
#include "LCD1602.h"

void DS1302_timeInit(void);
/***************************************
����Ĭ��ʱ�䡣˳��Ϊ ���ʱ���������� 2001��1��1�� ������ 00:00:00
ע�⣺����ҪΪBCD���ʽ��23=2+3=0x02+0x03=0x23
***************************************/
u8 Default_time[7]={0,0,0,1,1,5,1}; 


#define CLK PBout(5)
#define DAT_OUT PBout(7)
#define DAT_IN PBin(7)
#define RST PBout(6)

void DS1302_Init(void)
{
	RCC->APB2ENR|=1<<3;        //ʹ��PORTBʱ��
	GPIOB->CRL&=0X000FFFFF; 
	GPIOB->CRL|=0X73300000;   //PC0,PC2�������
	GPIOB->ODR&=0XFF1F; 	 	 //��ʼȫ���õ�
	
	DS1302_timeInit();//ʱ��ֵ��ʼ��
}

//DS1302дһ�ֽں���
void DS1302WriteByte(u8 dat)
{
	u8 i;
	CLK=0;
	delay_us(2);
	for(i=0;i<8;i++)
	{
		DAT_OUT=dat&0x01;
		delay_us(2);
		CLK=1;
		delay_us(2);
		CLK=0;
		dat>>=1;
	}
}
//DS1302��һ�ֽں���
u8 DS1302ReadByte()
{
	u8 i,dat;
	delay_us(2);
	for(i=0;i<8;i++)
	{
		dat>>=1;
		if(DAT_IN==1)
			dat|=0x80;
		CLK=1;
		delay_us(2);
		CLK=0;
		delay_us(2);
	}
	return dat;
}

//DS1302���Ĵ���
u8 DS1302ReadRegister(u8 reg)
{
    u8 temp; 
    CLK=0;
    RST=1;   
    DS1302WriteByte(reg);
    temp = DS1302ReadByte(); 
    CLK=1;
    RST=0;   
    return temp;
}

//DS1302д����
void DS1302WriteRegister(u8 reg, u8 value)
{
	RST=0;
	CLK=0;
	RST=1;
	DS1302WriteByte(reg);
	DS1302WriteByte(value);
	CLK=1;
	RST=0;
}

//��ʮ����ѹ��ΪBCD���ʽ
void DecToBCD(u8 *data)
{
	u8 i,temp;
	for(i=0;i<8;i++)
	{
		temp=data[i];
		data[i]=((temp/10)<<4)+((temp%10)&0x0F);
	}
}
//��BCD�뻹ԭΪʮ��������
void BCDToDec(u8 *data)
{
	u8 i,temp;
	for(i=0;i<8;i++)
	{
		temp=data[i];
		data[i]=((temp&0xF0)>>4)*10+(temp&0x0F);
	}
}

//DS1302�������ڼ�ʱ��
void DS1302SetTime(u8 *data)
{
    u8 i;
	  DecToBCD(data);
    CLK=0;
    RST=1;
    DS1302WriteByte(0xbe);	
    for (i = 0; i < 8; i++)	//˳��Ϊ ���ʱ��������
    {
        DS1302WriteByte(data[i]);
    }  
    CLK=0;
    RST=0;
}

//��ȡDS1302���ʱ�Ӽ�����
void DS1302ReadTime(u8 *buffer)
{
    u8 i;

    CLK=0;
    RST=1;
    DS1302WriteByte(0xbf); 
    for (i = 0; i < 8; i++)	//˳��Ϊ ���ʱ��������
    {
        buffer[i] = DS1302ReadByte();
    }  
    CLK=0;
    RST=0;
		BCDToDec(buffer);
}

//��ʼ��DS1302�����ʱ������ֵ
void DS1302_timeInit()
{
	u8 temp;
	temp = DS1302ReadRegister(0xc1); //��ȡ1302RAM���Դ�ֵ��ȷ���Ƿ��һ���ϵ�
	if(temp != 0xaa)
	{
		DS1302WriteRegister(0x8e, 0x00); //�ر�д����
		DS1302WriteRegister(0xc0, 0xaa); //��RAM����0xaa���´��ϵ�Ͳ�����д��ʼ������
		DS1302SetTime(Default_time);	
		DS1302WriteRegister(0x8e, 0x80);//��д����
	}
} 
//DS1302�������ڼ�ʱ��
void DS1302UpdateTime(u8 *data)
{
	DS1302WriteRegister(0x8e, 0x00); //�ر�д����
	DS1302WriteRegister(0xc0, 0xaa); //��RAM����0xaa���´��ϵ�Ͳ�����д��ʼ������
	DS1302SetTime(data);
	DS1302WriteRegister(0x8e, 0x80);//��д����
}


