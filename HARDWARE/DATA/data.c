#include "stdio.h"
#include "delay.h"
#include "data.h"
#include "LCD1602.h"
#include "flash.h"
#include "DS1302.h"
#include "adc.h"

//����ɼ���ĸ���
#define SAMPLES_NUM 1
#define SAMPLE_count 10 //ÿ����ƽ���ɼ�����
//����ֵ�����С֮����ֵ
#define A 1
//���ڲ�����֮����ֵ
#define B 100
//������ʱ����������
u8 pBuffer[2];    
u8 qBuffer[2];
//����ɼ������ݻ�����
u16 DATA_Buffer[100];
u8  TIME_Buffer[7];
//�洢AD�ɼ���������
u16  AD9200_data;
//���弫��
u8   POLAR;
//��ֵ�˲��ñ���
u16  Max_data;
u16  Min_data;
//����������ַ 0~2047 
u16  Sector_addr=0;	
//���ݴ洢ָ���ʵ�ʵ�ַ
u32  Flash_addr=0;	
//�ⲿ�жϷ�������ı�������
u8 counts;
//�����õı���
u16  offset=0;

/********��ȡ�ɼ������׻���������洢�ڵ�2047��������(��ַ��8384512)********/
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

/********�������ݺ���********/
//TIPS:��һ������δʹ��
void DATASAVE_inCHIP(int exti)
{
	u8 k;
	Flash_addr=Sector_addr*4096; 	//����Ҫ�����ݵĵ�ַ

	for(k=0;k<SAMPLES_NUM;k++)														//��������
	{ 
		pBuffer[0]=DATA_Buffer[k]>>8;
		pBuffer[1]=DATA_Buffer[k]&0xff;
		SPI_Flash_Write_NoCheck(pBuffer,Flash_addr,2);
		Flash_addr+=2;
	}	
/*****�����д����ɼ������ڼ�ʱ���Լ����Եĺ���*********/
	DS1302ReadTime(TIME_Buffer); //��ȡ��ʵʱʱ��
	SPI_Flash_Write_NoCheck(TIME_Buffer,8372224+(Sector_addr-1)*8,7);  //��ʱ���¼���룬Ԥ��һ�ֽڴ洢���Ե�λ��
	if(exti==1)					//����
		POLAR=1;
	else if(exti==0)
		POLAR=0;
	SPI_Flash_Write_NoCheck(&POLAR,8372223+(Sector_addr-1)*8,1);   //�洢����

	Sector_addr++; 																//�����׻�����
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

/********��ȡָ��λ���ϵ�����********/
u16 Data_Read(u32 addr)
{
	s8 i;
	u16 data=0;
	SPI_Flash_Read(pBuffer,addr,2);		//8384512λ�洢��ַ��������ַ����ַһ��2�ֽ�
	for(i=1;i>=0;i--)
	{									 
		data+=(pBuffer[1-i]<<(i*8));		  
	}
	return data;								 
}

/********��ȡоƬ���Ѿ��洢�˵����ݵ������Сֵ********/
//TIPS����оƬ��������Ǵӵڶ���������ʼ�洢��
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

/********оƬ���ݲ���********/
void EREASE_CHIP()
{
	SPI_Flash_Erase_Chip();		   
	pBuffer[0]=0;				   //д��ַ
	pBuffer[1]=1;
	SPI_Flash_Write_NoCheck(pBuffer,8384512,2); //��ʼ��������ַ
	Sector_addr=Sector_addr_READ();	
}


/********�洢����ϵͳ��ʼ��********/
void SAVEDATA_SYSTEM_Init()
{
	Sector_addr=Sector_addr_READ(); //��ȡĿǰ���ݴ洢��������λ��
}
