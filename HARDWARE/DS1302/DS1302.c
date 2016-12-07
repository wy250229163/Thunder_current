#include "delay.h"
#include "DS1302.h"
#include "LCD1602.h"

void DS1302_timeInit(void);
/***************************************
设置默认时间。顺序为 秒分时日月星期年 2001年1月1日 星期六 00:00:00
注意：数据要为BCD码格式！23=2+3=0x02+0x03=0x23
***************************************/
u8 Default_time[7]={0,0,0,1,1,5,1}; 


#define CLK PBout(5)
#define DAT_OUT PBout(7)
#define DAT_IN PBin(7)
#define RST PBout(6)

void DS1302_Init(void)
{
	RCC->APB2ENR|=1<<3;        //使能PORTB时钟
	GPIOB->CRL&=0X000FFFFF; 
	GPIOB->CRL|=0X73300000;   //PC0,PC2推挽输出
	GPIOB->ODR&=0XFF1F; 	 	 //初始全部置低
	
	DS1302_timeInit();//时间值初始化
}

//DS1302写一字节函数
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
//DS1302读一字节函数
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

//DS1302读寄存器
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

//DS1302写数据
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

//将十进制压缩为BCD码格式
void DecToBCD(u8 *data)
{
	u8 i,temp;
	for(i=0;i<8;i++)
	{
		temp=data[i];
		data[i]=((temp/10)<<4)+((temp%10)&0x0F);
	}
}
//将BCD码还原为十进制数据
void BCDToDec(u8 *data)
{
	u8 i,temp;
	for(i=0;i<8;i++)
	{
		temp=data[i];
		data[i]=((temp&0xF0)>>4)*10+(temp&0x0F);
	}
}

//DS1302设置日期及时钟
void DS1302SetTime(u8 *data)
{
    u8 i;
	  DecToBCD(data);
    CLK=0;
    RST=1;
    DS1302WriteByte(0xbe);	
    for (i = 0; i < 8; i++)	//顺序为 秒分时日月周年
    {
        DS1302WriteByte(data[i]);
    }  
    CLK=0;
    RST=0;
}

//读取DS1302里的时钟及日期
void DS1302ReadTime(u8 *buffer)
{
    u8 i;

    CLK=0;
    RST=1;
    DS1302WriteByte(0xbf); 
    for (i = 0; i < 8; i++)	//顺序为 秒分时日月周年
    {
        buffer[i] = DS1302ReadByte();
    }  
    CLK=0;
    RST=0;
		BCDToDec(buffer);
}

//初始化DS1302里面的时间日期值
void DS1302_timeInit()
{
	u8 temp;
	temp = DS1302ReadRegister(0xc1); //读取1302RAM，以此值来确定是否第一次上电
	if(temp != 0xaa)
	{
		DS1302WriteRegister(0x8e, 0x00); //关闭写保护
		DS1302WriteRegister(0xc0, 0xaa); //将RAM存入0xaa，下次上电就不会重写初始日期了
		DS1302SetTime(Default_time);	
		DS1302WriteRegister(0x8e, 0x80);//打开写保护
	}
} 
//DS1302更新日期及时钟
void DS1302UpdateTime(u8 *data)
{
	DS1302WriteRegister(0x8e, 0x00); //关闭写保护
	DS1302WriteRegister(0xc0, 0xaa); //将RAM存入0xaa，下次上电就不会重写初始日期了
	DS1302SetTime(data);
	DS1302WriteRegister(0x8e, 0x80);//打开写保护
}


