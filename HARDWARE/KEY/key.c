#include "key.h"
#include "delay.h"
#include "LCD1602.h"
#include "data.h"
#include "flash.h"
#include "DS1302.h"

//�������ݲɼ�C�ļ���ı�����Ϊ�׻������ļ���
extern u16 Sector_addr;
//����洢��ʼʱ���оƬ��ַ
#define addr_STARTTIME 8384500
//��ʾ���л�����
#define disp_queue_sum 16
char disp_queue_1[disp_queue_sum];
char disp_queue_2[disp_queue_sum];
//����������ݵ�����
u8 REAL_PASSWORD[6]={2,1,3,2,4,3};
u8 INPUT_PASSWORD[6];
u8 INPUT_PASSWORD_FLAG;
u8 INPUT_COUNTS;
//�洢ʵʱʱ�ӵ����顣˳��Ϊ ���ʱ����������
u8 TIME_buffer[7];
u8 STARTTIME_buffer[7];  
u8 HISTORYTIME_buffer[7];			//���ʱ����������
//�������ڼ�ʱ��ṹ��
struct time
{
	//����ϵͳ
	u8 YEAR;
	u8 MONTH;
	u8 DAY;
	//ʱ��ϵͳ
	u8 HOUR;
	u8 MINUTES;
	u8 SECONDS;
}REAL_TIME,START_TIME;
//������ʷ���ݽṹ��
struct history
{
	u16 COUNTS;
	u16 PEAK;
	u8 POLAR;
	struct time TIME;
}HISTORY_DATA;
//�׵����
u16 THUNDER_COUNTS;
//�޼�������
u8 NOKEY_JSQ;
//��ʱ���������
u8 TIMER2_COUNTS;
//����ģʽ
u8 WORK_MODE;
//��ʾ����ϵͳ
void display_DATE_Y()
{
	disp_queue_2[1]=0x30+REAL_TIME.YEAR/10;
	disp_queue_2[2]=0x30+REAL_TIME.YEAR%10;
}
void display_DATE_M()
{
	disp_queue_2[4]=0x30+REAL_TIME.MONTH/10;
	disp_queue_2[5]=0x30+REAL_TIME.MONTH%10;
}
void display_DATE_D()
{
	disp_queue_2[7]=0x30+REAL_TIME.DAY/10;
	disp_queue_2[8]=0x30+REAL_TIME.DAY%10;
}
//��ʾʱ��ϵͳ
void display_TIME_H()
{
	disp_queue_2[10]=0x30+REAL_TIME.HOUR/10;
	disp_queue_2[11]=0x30+REAL_TIME.HOUR%10;
}
void display_TIME_M()
{
	disp_queue_2[13]=0x30+REAL_TIME.MINUTES/10;
	disp_queue_2[14]=0x30+REAL_TIME.MINUTES%10;
}

//��ʾ��¼��ʼ����ϵͳ
void display_START_DATE_Y()
{
	disp_queue_2[1]=0x30+START_TIME.YEAR/10;
	disp_queue_2[2]=0x30+START_TIME.YEAR%10;
}
void display_START_DATE_M()
{
	disp_queue_2[4]=0x30+START_TIME.MONTH/10;
	disp_queue_2[5]=0x30+START_TIME.MONTH%10;
}
void display_START_DATE_D()
{
	disp_queue_2[7]=0x30+START_TIME.DAY/10;
	disp_queue_2[8]=0x30+START_TIME.DAY%10;
}
//��ʾ��¼��ʼʱ��ϵͳ
void display_START_TIME_H()
{
	disp_queue_2[10]=0x30+START_TIME.HOUR/10;
	disp_queue_2[11]=0x30+START_TIME.HOUR%10;
}
void display_START_TIME_M()
{
	disp_queue_2[13]=0x30+START_TIME.MINUTES/10;
	disp_queue_2[14]=0x30+START_TIME.MINUTES%10;
}
//��ʾ��ʷ��¼����ϵͳ
void display_HISTORY_DATE_Y()
{
	disp_queue_2[1]=0x30+HISTORY_DATA.TIME.YEAR/10;
	disp_queue_2[2]=0x30+HISTORY_DATA.TIME.YEAR%10;
}
void display_HISTORY_DATE_M()
{
	disp_queue_2[4]=0x30+HISTORY_DATA.TIME.MONTH/10;
	disp_queue_2[5]=0x30+HISTORY_DATA.TIME.MONTH%10;
}
void display_HISTORY_DATE_D()
{
	disp_queue_2[7]=0x30+HISTORY_DATA.TIME.DAY/10;
	disp_queue_2[8]=0x30+HISTORY_DATA.TIME.DAY%10;
}
//��ʾ��ʷ��¼ʱ��ϵͳ
void display_HISTORY_TIME_H()
{
	disp_queue_2[10]=0x30+HISTORY_DATA.TIME.HOUR/10;
	disp_queue_2[11]=0x30+HISTORY_DATA.TIME.HOUR%10;
}
void display_HISTORY_TIME_M()
{
	disp_queue_2[13]=0x30+HISTORY_DATA.TIME.MINUTES/10;
	disp_queue_2[14]=0x30+HISTORY_DATA.TIME.MINUTES%10;
}
//��ʾ�ܵ��׵����
void display_THUNDER_COUNTS()
{
	THUNDER_COUNTS=Sector_addr-1;  //��Ϊ��data.c�ļ��Sector_addrʵ���ϴ�����ǽ�Ҫ�ɼ���X�����ݣ���������Ҫ��һ
	disp_queue_1[9] =0x30+THUNDER_COUNTS/1000;
	disp_queue_1[10]=0x30+THUNDER_COUNTS%1000/100;
	disp_queue_1[11]=0x30+THUNDER_COUNTS%100/10;
	disp_queue_1[12]=0x30+THUNDER_COUNTS%10;

}
//��ʾ��ʷ��¼ʱ�Ĵ���
void display_TEMP_COUNTS()
{
	disp_queue_1[9] =0x30+HISTORY_DATA.COUNTS/1000;
	disp_queue_1[10]=0x30+HISTORY_DATA.COUNTS%1000/100;
	disp_queue_1[11]=0x30+HISTORY_DATA.COUNTS%100/10;
	disp_queue_1[12]=0x30+HISTORY_DATA.COUNTS%10;

}
//��ʾ��������ʱ���ǡ�XX-XX-XX XX:XX��
void display_ALL1()
{
	disp_queue_2[1]=' ';
	disp_queue_2[2]=' ';	
}
void display_ALL2()
{
	disp_queue_2[4]=' ';
	disp_queue_2[5]=' ';	
}
void display_ALL3()
{
	disp_queue_2[7]=' ';
	disp_queue_2[8]=' ';	
}
void display_ALL4()
{
	disp_queue_2[10]=' ';
	disp_queue_2[11]=' ';	
}
void display_ALL5()
{
	disp_queue_2[13]=' ';
	disp_queue_2[14]=' ';	
}
//��ʾ�������ѡ���ǡ�YES XX�������ǡ�XXX NO��
void display_ALL6()
{
	disp_queue_2[3]=' ';
	disp_queue_2[4]=' ';
	disp_queue_2[5]=' ';
	disp_queue_2[11]='N';
	disp_queue_2[12]='O';		
}
void display_ALL7()
{
	disp_queue_2[3]='Y';
	disp_queue_2[4]='E';
	disp_queue_2[5]='S';
	disp_queue_2[11]=' ';
	disp_queue_2[12]=' ';	
}
//��ʾ���ڷָ���
void display_LINE()
{
	disp_queue_2[3]='-';
	disp_queue_2[6]='-';
}
//��ʾʱ��ָ���
void display_COL()
{
	disp_queue_2[12]=':';
}
//����ʾʱ��ָ���
void display_NOCOL()
{
	disp_queue_2[12]=' ';
}
//ʱ��������ʾ�ո�
void display_SPACE()
{
	disp_queue_2[0]=' ';
	disp_queue_2[9]=' ';
	disp_queue_2[15]=' ';
}
//��ʾ����״̬�ַ�
void display_WAITWORDS()
{
	disp_queue_1[0]=' ';
	disp_queue_1[1]=' ';
	disp_queue_1[2]='T';
	disp_queue_1[3]='O';
	disp_queue_1[4]='T';
	disp_queue_1[5]='A';
	disp_queue_1[6]='L';
	disp_queue_1[7]=':';
	disp_queue_1[8]=' ';
	disp_queue_1[13]=' ';
	disp_queue_1[14]=' ';
	disp_queue_1[15]=' ';
}
//��ʾ��¼��ʼʱ���ַ�
void display_STARTTIMEWORDS()
{
	disp_queue_1[0]=' ';
	disp_queue_1[1]=' ';
	disp_queue_1[2]='S';
	disp_queue_1[3]='T';
	disp_queue_1[4]='A';
	disp_queue_1[5]='R';
	disp_queue_1[6]='T';
	disp_queue_1[7]=' ';
	disp_queue_1[8]='T';
	disp_queue_1[9]='I';
	disp_queue_1[10]='M';
	disp_queue_1[11]='E';
	disp_queue_1[12]=':';
	disp_queue_1[13]=' ';
	disp_queue_1[14]=' ';
	disp_queue_1[15]=' ';
}
//��ʾ��ʷ��¼�ַ�
void display_HISTORYDATAWORDS()
{
	disp_queue_1[0]=' ';
	disp_queue_1[1]='H';
	disp_queue_1[2]='I';
	disp_queue_1[3]='S';
	disp_queue_1[4]='T';
	disp_queue_1[5]='O';
	disp_queue_1[6]='R';
	disp_queue_1[7]='Y';
	disp_queue_1[8]=' ';
	disp_queue_1[9]=' ';
	disp_queue_1[10]='D';
	disp_queue_1[11]='A';
	disp_queue_1[12]='T';
	disp_queue_1[13]='A';
	disp_queue_1[14]='?';
	disp_queue_1[15]=' ';
	disp_queue_2[0]=' ';
	disp_queue_2[1]=' ';
	disp_queue_2[2]=' ';
	disp_queue_2[3]=' ';
	disp_queue_2[4]=' ';
	disp_queue_2[5]=' ';
	disp_queue_2[6]='[';
	disp_queue_2[7]='O';
	disp_queue_2[8]='K';
	disp_queue_2[9]=']';
	disp_queue_2[10]=' ';
	disp_queue_2[11]=' ';
	disp_queue_2[12]=' ';
	disp_queue_2[13]=' ';
	disp_queue_2[14]=' ';
	disp_queue_2[15]=' ';
}
//��ʾδ������ʷ��¼ʱ�ַ�
void display_NULL_HISTORYDATAWORDS()
{
	disp_queue_1[0]=' ';
	disp_queue_1[1]=' ';
	disp_queue_1[2]=' ';
	disp_queue_1[3]=' ';
	disp_queue_1[4]=' ';
	disp_queue_1[5]=' ';
	disp_queue_1[6]=' ';
	disp_queue_1[7]='N';
	disp_queue_1[8]='O';
	disp_queue_1[9]=' ';
	disp_queue_1[10]=' ';
	disp_queue_1[11]=' ';
	disp_queue_1[12]=' ';
	disp_queue_1[13]=' ';
	disp_queue_1[14]=' ';
	disp_queue_1[15]=' ';
	disp_queue_2[0]=' ';
	disp_queue_2[1]=' ';
	disp_queue_2[2]='H';
	disp_queue_2[3]='I';
	disp_queue_2[4]='S';
	disp_queue_2[5]='T';
	disp_queue_2[6]='O';
	disp_queue_2[7]='R';
	disp_queue_2[8]='Y';
	disp_queue_2[9]=' ';
	disp_queue_2[10]='D';
	disp_queue_2[11]='A';
	disp_queue_2[12]='T';
	disp_queue_2[13]='A';
	disp_queue_2[14]=' ';
	disp_queue_2[15]=' ';
}
//��ʾ��ʷ�����ַ�
void display_HISTORYDATA()
{
	u16 temp;
	temp=(double)HISTORY_DATA.PEAK/4096*330;
	
	disp_queue_1[0]='N';
	disp_queue_1[1]='O';
	disp_queue_1[2]='.';
	disp_queue_1[3]=0x30+HISTORY_DATA.COUNTS/1000;
	disp_queue_1[4]=0x30+HISTORY_DATA.COUNTS%1000/100;
	disp_queue_1[5]=0x30+HISTORY_DATA.COUNTS%100/10;
	disp_queue_1[6]=0x30+HISTORY_DATA.COUNTS%10;
	disp_queue_1[7]=' ';
	disp_queue_1[8]=' ';
	if(HISTORY_DATA.POLAR == 1)				//������ʾ
		disp_queue_1[9]='+';
	else
		disp_queue_1[9]='-';
	disp_queue_1[10]=0x30+(u8)(temp/100);
	disp_queue_1[11]='.';
	disp_queue_1[12]=0x30+(u8)(temp%100/10);
	disp_queue_1[13]=0x30+(u8)(temp%10);
	disp_queue_1[14]='k';
	disp_queue_1[15]='V';
	
	display_HISTORY_DATE_Y();
	display_HISTORY_DATE_M();
	display_HISTORY_DATE_D();
	display_HISTORY_TIME_H();
	display_HISTORY_TIME_M();
	display_LINE();
	display_COL();
	display_SPACE();
	
}
//��ʾɾ��ȷ���ַ�
void display_DELETEDATAWORDS()
{
	disp_queue_1[0]='D';
	disp_queue_1[1]='E';
	disp_queue_1[2]='L';
	disp_queue_1[3]='E';
	disp_queue_1[4]='T';
	disp_queue_1[5]='E';
	disp_queue_1[6]=' ';
	disp_queue_1[7]='A';
	disp_queue_1[8]='L';
	disp_queue_1[9]='L';
	disp_queue_1[10]=' ';
	disp_queue_1[11]='D';
	disp_queue_1[12]='A';
	disp_queue_1[13]='T';
	disp_queue_1[14]='A';
	disp_queue_1[15]='?';
	disp_queue_2[0]=' ';
	disp_queue_2[1]=' ';
	disp_queue_2[2]='[';
	disp_queue_2[3]=' ';
	disp_queue_2[4]=' ';
	disp_queue_2[5]=' ';
	disp_queue_2[6]=']';
	disp_queue_2[7]=' ';
	disp_queue_2[8]=' ';
	disp_queue_2[9]=' ';
	disp_queue_2[10]='[';
	disp_queue_2[11]='N';
	disp_queue_2[12]='O';
	disp_queue_2[13]=']';
	disp_queue_2[14]=' ';
	disp_queue_2[15]=' ';
}
//��ʾ��ʾ���������ַ�
void display_ENTERPASSWORD()
{
	disp_queue_1[0]='E';
	disp_queue_1[1]='N';
	disp_queue_1[2]='T';
	disp_queue_1[3]='E';
	disp_queue_1[4]='R';
	disp_queue_1[5]=' ';
	disp_queue_1[6]='P';
	disp_queue_1[7]='A';
	disp_queue_1[8]='S';
	disp_queue_1[9]='S';
	disp_queue_1[10]='W';
	disp_queue_1[11]='O';
	disp_queue_1[12]='R';
	disp_queue_1[13]='D';
	disp_queue_1[14]=':';
	disp_queue_1[15]=' ';
}
//���������ַ�
void redisplay_PASSWORD()
{
		LCDwrite_char(0,1,' ');
		LCDwrite_char(1,1,' ');
		LCDwrite_char(2,1,' ');
		LCDwrite_char(3,1,' ');
		LCDwrite_char(4,1,' ');
		LCDwrite_char(5,1,'-');
		LCDwrite_char(6,1,'-');
		LCDwrite_char(7,1,'-');
		LCDwrite_char(8,1,'-');
		LCDwrite_char(9,1,'-');
		LCDwrite_char(10,1,'-');
		LCDwrite_char(11,1,' ');
		LCDwrite_char(12,1,' ');
		LCDwrite_char(13,1,' ');
		LCDwrite_char(14,1,' ');
		LCDwrite_char(15,1,' ');		
}
//��ʾɾ��ȷ���ַ�
void display_DELETEWAITWORDS()
{
		LCDwrite_char(0,0,' ');
		LCDwrite_char(1,0,'D');
		LCDwrite_char(2,0,'E');
		LCDwrite_char(3,0,'L');
		LCDwrite_char(4,0,'E');
		LCDwrite_char(5,0,'T');
		LCDwrite_char(6,0,'I');
		LCDwrite_char(7,0,'N');
		LCDwrite_char(8,0,'G');
		LCDwrite_char(9,0,' ');
		LCDwrite_char(10,0,'D');
		LCDwrite_char(11,0,'A');
		LCDwrite_char(12,0,'T');
		LCDwrite_char(13,0,'A');
		LCDwrite_char(14,0,'!');
		LCDwrite_char(15,0,' ');	
}


void Parameter_Init() //������ʼ��
{
	HISTORY_DATA.POLAR=1;
	
	THUNDER_COUNTS=Sector_addr;
	NOKEY_JSQ=0;
	TIMER2_COUNTS=0;
	WORK_MODE=0;
	INPUT_PASSWORD_FLAG=0;
	INPUT_COUNTS=0;
}


void KEY_Init(void) //������ʼ������ PA0��PC0/1/2 ���ó�����
{
	RCC->APB2ENR|=1<<2;     //ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<4;     //ʹ��PORTCʱ��
	
	GPIOA->CRL&=0XFFFFFFF0;	//PA0���ó�����	  
	GPIOA->CRL|=0X00000008;
	GPIOA->ODR|=0X01;	   	//PA0����
	GPIOC->CRL&=0XFFFFF000;	//PC0/1/2���ó�����	  
	GPIOC->CRL|=0X00000888;   
	GPIOC->ODR|=0x07;	   	//PC0/1/2����
	
	Parameter_Init();
	if(WORK_MODE==0)
	{
		display_WAITWORDS();
		display_THUNDER_COUNTS();
		display_DATE_Y();
		display_DATE_M();
		display_DATE_D();
		display_TIME_H();
		display_TIME_M();
		display_LINE();
		display_SPACE();
		LCDwrite_str(0,0,(u8*)disp_queue_1);
		LCDwrite_str(0,1,(u8*)disp_queue_2);					
	}	
} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ0 û���κΰ�������
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
		else if(KEY4==0)return KEY4_PRES;
	}else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)key_up=1; 	     
	return 0;// �ް�������
}

void SAVE_DATEandTIME_DATA()   //���ڱ����޸ĺ������ʱ�����ݵĺ���
{
	TIME_buffer[6]=REAL_TIME.YEAR;		//��
	TIME_buffer[4]=REAL_TIME.MONTH;		//��
	TIME_buffer[3]=REAL_TIME.DAY;			//��
	TIME_buffer[2]=REAL_TIME.HOUR;		//ʱ
	TIME_buffer[1]=REAL_TIME.MINUTES; //��
	TIME_buffer[0]=0;								  //��
	DS1302UpdateTime(TIME_buffer);
}

void UPDATA_HISTORYDATA_DISPLAY(u8 t)	//�����޸ġ�������ʷ������ʾ״̬ʱ����ʾ���ݵĺ���
{
	if(t)
	{
		HISTORY_DATA.COUNTS++;
		if(HISTORY_DATA.COUNTS > THUNDER_COUNTS)
			HISTORY_DATA.COUNTS=1;
	}
	else
	{
		HISTORY_DATA.COUNTS--;
		if(HISTORY_DATA.COUNTS < 1)
			HISTORY_DATA.COUNTS=THUNDER_COUNTS;
	}
	SPI_Flash_Read(&HISTORY_DATA.POLAR,8372223+(HISTORY_DATA.COUNTS-1)*8,1); //������
	HISTORY_DATA.PEAK=GET_INchip_MAXdata(HISTORY_DATA.COUNTS);	//�ҷ�ֵ*********************************************************
	SPI_Flash_Read(HISTORYTIME_buffer,8372224+(HISTORY_DATA.COUNTS-1)*8,7);   //��ȡ��ʷ��¼����ʱ��
	HISTORY_DATA.TIME.YEAR=HISTORYTIME_buffer[6];
	HISTORY_DATA.TIME.MONTH=HISTORYTIME_buffer[4];
	HISTORY_DATA.TIME.DAY=HISTORYTIME_buffer[3];
	HISTORY_DATA.TIME.HOUR=HISTORYTIME_buffer[2];
	HISTORY_DATA.TIME.MINUTES=HISTORYTIME_buffer[1];
}

void CHEAK_PASSWPRD()
{
	u8 i;
	u8 FALSE=0;
	INPUT_COUNTS=0;
	for(i=0;i<6;i++)
	{
		if(INPUT_PASSWORD[i]!=REAL_PASSWORD[i])
			{FALSE=1;break;}		
	}
	if(FALSE)
	{	
		WORK_MODE=12;
		redisplay_PASSWORD();//����������״̬��1602�ڶ�����������
	}
	else if(!FALSE&&INPUT_PASSWORD_FLAG==1)
	{
		INPUT_PASSWORD_FLAG=0;
		LCD_clear();
		delay_ms(10);
		WORK_MODE=13;
	}
}

void UPDATA_display()  //��������ʱ����ʾ
{
		LCDwrite_str(0,1,(u8*)disp_queue_2);
}

//��ʱ��2�жϷ����������ʱ��
void TIM2_IRQHandler(void)
{ 		    		  			    
	if(TIM2->SR & 0X0001)//����ж�
	{
			TIMER2_COUNTS++;
			if(TIMER2_COUNTS%2 == 0)
			{
				TIMER2_COUNTS = 0;
				if(WORK_MODE==0)
				{
					display_NOCOL();
					UPDATA_display();
					DS1302ReadTime(TIME_buffer); //1s�Ӷ�һ��DS1302������
				}
			}
			else
			{
				if(WORK_MODE==0)
				{
					display_COL();
					UPDATA_display();	
					//1s���޸�һ��ʵʱʱ��
					REAL_TIME.YEAR=TIME_buffer[6];		//��
					REAL_TIME.MONTH=TIME_buffer[4];		//��
					REAL_TIME.DAY=TIME_buffer[3];			//��
					REAL_TIME.HOUR=TIME_buffer[2];		//ʱ
					REAL_TIME.MINUTES=TIME_buffer[1]; //��
				}
			}
			if(NOKEY_JSQ>60)
			{
				WORK_MODE=0;
				INPUT_PASSWORD_FLAG = 0;
			}
			else
			{
				NOKEY_JSQ++;
			}
	}				   
	TIM2->SR&=~(1<<0);//����жϱ�־λ 	    
}

void KEY1_Handler()
{
		NOKEY_JSQ=0;	
		if(WORK_MODE == 12 && INPUT_PASSWORD_FLAG == 1)		//���봦������
		{
			INPUT_PASSWORD[INPUT_COUNTS] = 1;
			LCDwrite_char(5+INPUT_COUNTS,1,'*');
			INPUT_COUNTS++;
		}
		if(INPUT_COUNTS >= 6)
			CHEAK_PASSWPRD();

		if(!INPUT_PASSWORD_FLAG)			//�ڷ���������״̬ʱ�ż��״̬
		{		
			switch(WORK_MODE)
			{
				case 0:								//����״̬
					SPI_Flash_Read(STARTTIME_buffer,addr_STARTTIME,7);   //��ȡ��ʼ��¼ʱ��
					START_TIME.YEAR=STARTTIME_buffer[6];
					START_TIME.MONTH=STARTTIME_buffer[4];
					START_TIME.DAY=STARTTIME_buffer[3];
					START_TIME.HOUR=STARTTIME_buffer[2];
					START_TIME.MINUTES=STARTTIME_buffer[1];
					WORK_MODE=1;
					break;
				case 1:								//��¼��ʼʱ��״̬
					WORK_MODE=2;
					break;
				case 2:								//ȷ���Ƿ��ѯ��ʷ����״̬
					WORK_MODE=3;
					break;
				case 3:								//�����¼״̬
					WORK_MODE=0;
					break;
				case 12:							//��������״̬������Ӧ
				case 13:							//����ɾ������״̬������Ӧ
				case 14:							//��ʾû����ʷ����״̬������Ӧ
					break;
				default:							//ʵ��������״̬�·��ش���״̬
					WORK_MODE=0;
			}
		}	
}

void KEY2_Handler()
{
		NOKEY_JSQ=0;
		if(WORK_MODE == 12&&INPUT_PASSWORD_FLAG == 1)		//���봦������
		{
			INPUT_PASSWORD[INPUT_COUNTS] = 2;
			LCDwrite_char(5+INPUT_COUNTS,1,'*');
			INPUT_COUNTS++;
		}
		if(INPUT_COUNTS >= 6)
			CHEAK_PASSWPRD();
	
		if(!INPUT_PASSWORD_FLAG)			//�ڷ���������״̬ʱ�ż��״̬
		{		
			switch(WORK_MODE)
			{
				case 0:								//������״̬
					WORK_MODE=5;	
					display_DATE_Y();			
					display_ALL2();
					display_ALL3();
					display_ALL4();
					display_COL();
					display_ALL5();
					UPDATA_display();
					break;
				case 2:								//��ʷ��ѯ״̬
					if(THUNDER_COUNTS != 0)
					{
						//����״̬�л�ʱ����ʱ������ȫΪ0
						SPI_Flash_Read(&HISTORY_DATA.POLAR,8372223+(HISTORY_DATA.COUNTS-1)*8,1); //������
					  HISTORY_DATA.PEAK=GET_INchip_MAXdata(HISTORY_DATA.COUNTS);	//�ҷ�ֵ **********************************************
						SPI_Flash_Read(HISTORYTIME_buffer,8372224+(HISTORY_DATA.COUNTS-1)*8,7);   //��ȡ��ʷ��¼
						HISTORY_DATA.TIME.YEAR=HISTORYTIME_buffer[6];
						HISTORY_DATA.TIME.MONTH=HISTORYTIME_buffer[4];
						HISTORY_DATA.TIME.DAY=HISTORYTIME_buffer[3];
						HISTORY_DATA.TIME.HOUR=HISTORYTIME_buffer[2];
						HISTORY_DATA.TIME.MINUTES=HISTORYTIME_buffer[1];
						WORK_MODE=15;
					}
					else
						WORK_MODE=14;
					break;
				case 3:								//ѡ���Ƿ��������״̬
					WORK_MODE=0;
					break;
				case 5:								//������״̬
					WORK_MODE=6;	
					display_ALL1();
					display_DATE_M();
					display_ALL3();
					display_ALL4();
					display_COL();
					display_ALL5();
					UPDATA_display();
					break;
				case 6:								//������״̬
					WORK_MODE=7;	
					display_ALL1();
					display_ALL2();
					display_DATE_D();
					display_ALL4();
					display_COL();
					display_ALL5();
					UPDATA_display();
					break;
				case 7:								//����ʱ״̬
					WORK_MODE=8;	
					display_ALL1();
					display_ALL2();
					display_ALL3();
					display_TIME_H();
					display_COL();
					display_ALL5();
					UPDATA_display();
					break;	
				case 8:								//������״̬
					WORK_MODE=9;	
					display_ALL1();
					display_ALL2();
					display_ALL3();
					display_COL();
					display_TIME_M();
					display_ALL4();
					UPDATA_display();
					break;
				case 9:
					SAVE_DATEandTIME_DATA();		//������������
					LCD_clear();
					WORK_MODE=0;
					break;
				case 11:
					WORK_MODE=12;
					//��״̬��תʱ�������Ļ��ʾ����������״̬
					display_ENTERPASSWORD();
					redisplay_PASSWORD();
					LCDwrite_str(0,0,(u8*)disp_queue_1);
					INPUT_PASSWORD_FLAG=1;
					break;
				case 12:							//��������״̬������Ӧ
					break;
				default:
					break;
			}
		}	
}

void KEY3_Handler()
{
			NOKEY_JSQ=0;
			if(WORK_MODE == 12 && INPUT_PASSWORD_FLAG == 1)		//���봦������
			{
				INPUT_PASSWORD[INPUT_COUNTS]=3;
				LCDwrite_char(5+INPUT_COUNTS,1,'*');
				INPUT_COUNTS++;
			}
			if(INPUT_COUNTS >= 6)
				CHEAK_PASSWPRD();

			if(!INPUT_PASSWORD_FLAG)			//�ڷ���������״̬ʱ�ż��״̬
			{			
				switch(WORK_MODE)
				{
					//�������YES
					case 3:
						WORK_MODE=11;
						display_ALL7();
						UPDATA_display();
						break;
					//�����ռ���
					case 5:
						if(REAL_TIME.YEAR > 0)
							REAL_TIME.YEAR--;
						else
							REAL_TIME.YEAR=40;
						display_DATE_Y();
						UPDATA_display();
						break;
					case 6:
						if(REAL_TIME.MONTH > 0)
							REAL_TIME.MONTH--;
						else
							REAL_TIME.MONTH=12;
						display_DATE_M();
						UPDATA_display();
						break;
					case 7:
						if(REAL_TIME.DAY > 0)
							REAL_TIME.DAY--;
						else
							REAL_TIME.DAY=31;
						display_DATE_D();
						UPDATA_display();
						break;	
					//ʱ�ּ���	
					case 8:
						if(REAL_TIME.HOUR > 0)
							REAL_TIME.HOUR--;
						else
							REAL_TIME.HOUR=23;
						display_TIME_H();
						UPDATA_display();
						break;
					case 9:
						if(REAL_TIME.MINUTES > 0)
							REAL_TIME.MINUTES--;
						else
							REAL_TIME.MINUTES=59;
						display_TIME_M();
						UPDATA_display();
						break;
					case 12:							//��������״̬������Ӧ
						break;
					case 15:
						UPDATA_HISTORYDATA_DISPLAY(0);
						break;
					default:
						break;
				}
			}
}

void KEY4_Handler()
{
			NOKEY_JSQ=0;
			if(WORK_MODE == 12 && INPUT_PASSWORD_FLAG == 1)		//���봦������
			{
				INPUT_PASSWORD[INPUT_COUNTS]=4;
				LCDwrite_char(5+INPUT_COUNTS,1,'*');
				INPUT_COUNTS++;
			}
			if(INPUT_COUNTS >= 6)
				CHEAK_PASSWPRD();
			
			if(!INPUT_PASSWORD_FLAG)			//�ڷ���������״̬ʱ�ż��״̬
			{
				switch(WORK_MODE)
				{
					//����������
					case 5:
						if(REAL_TIME.YEAR < 40)
							REAL_TIME.YEAR++;
						else
							REAL_TIME.YEAR=0;
						display_DATE_Y();
						UPDATA_display();
						break;
					case 6:
						if(REAL_TIME.MONTH < 12)
							REAL_TIME.MONTH++;
						else
							REAL_TIME.MONTH=0;
						display_DATE_M();
						UPDATA_display();
						break;
					case 7:
						if(REAL_TIME.DAY < 31)
							REAL_TIME.DAY++;
						else
							REAL_TIME.DAY=0;
						display_DATE_D();
						UPDATA_display();
						break;	
					//ʱ������
					case 8:
						if(REAL_TIME.HOUR < 23)
							REAL_TIME.HOUR++;
						else
							REAL_TIME.HOUR=0;
						display_TIME_H();
						UPDATA_display();
						break;
					case 9:
						if(REAL_TIME.MINUTES < 59)
							REAL_TIME.MINUTES++;
						else
							REAL_TIME.MINUTES=0;
						display_TIME_M();
						UPDATA_display();
						break;
					//�������NO	
					case 11:
						WORK_MODE=3;
						display_ALL6();
						UPDATA_display();
						break;
					case 12:							//��������״̬������Ӧ
						break;
					case 15:
						UPDATA_HISTORYDATA_DISPLAY(1);
						break;
					default:
						break;
				}
			}
}


//������ʾ1602
void CHEAK_ISnewdisplay()
{
	u8 t;
	t=KEY_Scan(0);		//�õ���ֵ
	switch(t)
	{				 
			case KEY1_PRES:
				KEY1_Handler();
				break;
			case KEY2_PRES:
				KEY2_Handler();
				break;
			case KEY3_PRES:		
				KEY3_Handler();
				break;
			case KEY4_PRES:
				KEY4_Handler();
				break;
	}
	switch(WORK_MODE)
	{
			case 0:
				display_WAITWORDS();
				display_THUNDER_COUNTS();
				display_DATE_Y();
				display_DATE_M();
				display_DATE_D();
				display_TIME_H();
				display_TIME_M();
				display_LINE();
				display_SPACE();
				LCDwrite_str(0,0,(u8*)disp_queue_1);		//ˢ�µ�һ�ţ��ڶ����ڶ�ʱ�ж���ʱ�Ӹ���ʱ��ˢ��
				HISTORY_DATA.COUNTS=THUNDER_COUNTS;
				break;
			case 1:
				display_STARTTIMEWORDS();
				display_START_DATE_Y();
				display_START_DATE_M();
				display_START_DATE_D();
				display_START_TIME_H();
				display_START_TIME_M();
				display_LINE();
				display_COL();
				display_SPACE();
				LCDwrite_str(0,0,(u8*)disp_queue_1);
				LCDwrite_str(0,1,(u8*)disp_queue_2);				
				break;
			case 2:
				display_HISTORYDATAWORDS();
				LCDwrite_str(0,0,(u8*)disp_queue_1);
				LCDwrite_str(0,1,(u8*)disp_queue_2);
				break;
			case 3:
				display_DELETEDATAWORDS();
				LCDwrite_str(0,0,(u8*)disp_queue_1);
				LCDwrite_str(0,1,(u8*)disp_queue_2);
			break;
			case 12:
				break;
			case 13:
				display_DELETEWAITWORDS();
				EREASE_CHIP();
				SPI_Flash_Write_NoCheck(TIME_buffer,addr_STARTTIME,7); //����ʼ��¼ʱ��
				WORK_MODE=0;
				break;
			case 14:
				display_NULL_HISTORYDATAWORDS();
				LCDwrite_str(0,0,(u8*)disp_queue_1);
				LCDwrite_str(0,1,(u8*)disp_queue_2);
				delay_ms(1000);
				delay_ms(1000);
				WORK_MODE=0;
				break;
			case 15:
				display_HISTORYDATA();
				LCDwrite_str(0,0,(u8*)disp_queue_1);
				LCDwrite_str(0,1,(u8*)disp_queue_2);
				break;
	}
}






