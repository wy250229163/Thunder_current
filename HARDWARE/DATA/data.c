#include "stdio.h"
#include "delay.h"
#include "data.h"
#include "LCD1602.h"
#include "flash.h"
#include "DS1302.h"
#include "adc.h"

//定义采集点的个数
#define SAMPLES_NUM 1
#define SAMPLE_count 10 //每个点平均采集次数
//采样值最大最小之差阈值
#define A 1
//相邻采样点之差阈值
#define B 100
//储存临时变量缓冲区
u8 pBuffer[2];    
u8 qBuffer[2];
//定义采集的数据缓冲区
u16 DATA_Buffer[100];
u8  TIME_Buffer[7];
//存储AD采集到的数据
u16  AD9200_data;
//定义极性
u8   POLAR;
//幅值滤波用变量
u16  Max_data;
u16  Min_data;
//数据扇区地址 0~2047 
u16  Sector_addr=0;	
//数据存储指向的实际地址
u32  Flash_addr=0;	
//外部中断服务函数里的变量声明
u8 counts;
//调试用的变量
u16  offset=0;

/********读取采集到的雷击次数，其存储在第2047个扇区上(地址即8384512)********/
u16 Sector_addr_READ()
{
	s16 i=0;
	u16 data=0;
	do
	{
	 	SPI_Flash_Read(pBuffer,8384512+i*2,2);
		i++;
	}while((pBuffer[0]&pBuffer[1])^0xff);
	SPI_Flash_Read(pBuffer,8384512+(i-2)*2,2);
	for(i=1;i>=0;i--)
	{									 
		data+=(pBuffer[1-i]<<(i*8));
	}
	return data;								 
}

/********储存数据函数********/
//TIPS:第一个扇区未使用
void DATASAVE_inCHIP(int exti)
{
	u8 k;
	Flash_addr=Sector_addr*4096; 	//计算要存数据的地址

	for(k=0;k<SAMPLES_NUM;k++)														//储存数据
	{ 
		pBuffer[0]=DATA_Buffer[k]>>8;
		pBuffer[1]=DATA_Buffer[k]&0xff;
		SPI_Flash_Write_NoCheck(pBuffer,Flash_addr,2);
		Flash_addr+=2;
	}	
/*****这里编写储存采集的日期及时间以及极性的函数*********/
	DS1302ReadTime(TIME_Buffer); //读取到实时时钟
	SPI_Flash_Write_NoCheck(TIME_Buffer,8372224+(Sector_addr-1)*8,7);  //将时间记录存入，预留一字节存储极性的位置
	if(exti==1)					//极性
		POLAR=1;
	else if(exti==0)
		POLAR=0;
	SPI_Flash_Write_NoCheck(&POLAR,8372223+(Sector_addr-1)*8,1);   //存储极性

	Sector_addr++; 																//储存雷击次数
	Flash_addr=8384512+(Sector_addr-1)*2;				
	qBuffer[0]=Sector_addr>>8;
	qBuffer[1]=Sector_addr&0xff;
	SPI_Flash_Write_NoCheck(qBuffer,Flash_addr,2);
      
	if(Sector_addr>=2000)
	{
		Sector_addr=2000;
	}
}

void EXTI0_IRQHandler(void)
{
	u8 k;
	for(k=0;k<SAMPLES_NUM;k++)
	{
		DATA_Buffer[k]=Get_Adc_Average(ADC_CH1,SAMPLE_count);	
	}
	DATASAVE_inCHIP(0);
	EXTI->PR=1<<0;
} 

void EXTI1_IRQHandler(void)
{
	u8 k;
	for(k=0;k<SAMPLES_NUM;k++)
	{
		DATA_Buffer[k]=Get_Adc_Average(ADC_CH2,SAMPLE_count);	
	}
	DATASAVE_inCHIP(1);
	EXTI->PR=1<<1;
}

/********读取指定位置上的数据********/
u16 Data_Read(u32 addr)
{
	s8 i;
	u16 data=0;
	SPI_Flash_Read(pBuffer,addr,2);		//8384512位存储地址的扇区地址，地址一共2字节
	for(i=1;i>=0;i--)
	{									 
		data+=(pBuffer[1-i]<<(i*8));		  
	}
	return data;								 
}

/********读取芯片里已经存储了的数据的最大最小值********/
//TIPS：在芯片里的数据是从第二个扇区开始存储的
u16 GET_INchip_MAXdata(u16 sector)
{
	u8 i;
	u16 MAX,temp;
	MAX=Data_Read(sector*4096);
	for(i=0;i<SAMPLES_NUM;i++)
	{
		temp=Data_Read(sector*4096+i*2);
		if(MAX < temp)
			MAX=temp;
	}
	return MAX;
}

u16 GET_INchip_MINdata(u16 sector)
{
	u8 i;
	u16 MIN,temp;
	MIN=Data_Read(sector*4096);
	for(i=0;i<SAMPLES_NUM;i++)
	{
		temp=Data_Read(sector*4096+i*2);
		if(MIN > temp)
			MIN=temp;
	}
	return MIN;
}

/********芯片内容擦除********/
void EREASE_CHIP()
{
	SPI_Flash_Erase_Chip();		   
	pBuffer[0]=0;				   //写地址
	pBuffer[1]=1;
	SPI_Flash_Write_NoCheck(pBuffer,8384512,2); //初始化扇区地址
	Sector_addr=Sector_addr_READ();	
}


/********存储数据系统初始化********/
void SAVEDATA_SYSTEM_Init()
{
	Sector_addr=Sector_addr_READ(); //获取目前数据存储到的扇区位置
}
