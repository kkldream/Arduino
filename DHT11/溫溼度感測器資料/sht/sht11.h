 #ifndef __SHT11_H__
#define __SHT11_H__
/*************************
SHT11��?�R�O
**************************/
#define uchar unsigned char
#define uint  unsigned int
#define TEM_TEST         0x03//?��??�R�O
#define HUM_TEST          0x05//?��??�R�O
#define REG_READ          0x07//?�H�s��
#define REG_WRITE         0x06//?�H�s��
#define FUNCTION_SET    0x01//?�mSHT11���u�@���?8��/?�� 12��?�� /**************************

sbit SHT11_DATA=P3^1;
sbit SHT11_SCK=P3^0;
sbit P33=P3^3;
sbit P32=P3^2;
sbit P36=P3^6;
sbit P37=P3^7;
uchar flag_tempeture=0;  //?��?�צ�m��?��
uchar flag_humidity=0;     //?��?�צ�m��?��
uchar code str1[]={ 0x10,0x06,0x09,0x08,0x08,0x09,0x06,0x00};//?��??
uchar code str6_sht11[]="%RH   ";
uchar code str4_sht11[]="humi=";
uchar code str2_sht11[]="temp=";
uchar code str7_sht11[]="    ";//�M��?���n��?��



void Delay_Ms(uint ms)    // ms��?��? (AT89C51 @ 11.0592MHz)
{
uint i;
uchar j;
for(i=0;i<ms;i++)
{
for(j=0;j<200;j++);
for(j=0;j<102;j++);
}
}

void SHT11_Start()
{
SHT11_SCK=1;
SHT11_DATA=1;
Delay();
SHT11_DATA=0;
Delay();
SHT11_SCK=0;
Delay();
SHT11_SCK=1;
Delay();
SHT11_DATA=1;
}

void SHT11_Sendbyte(uchar dat)
{
uchar i;
SHT11_SCK=0;
Delay();
for(i=0;i<8;i++)
{
if(dat&0x80)
{
SHT11_DATA=1;
Delay();
}
else
{
SHT11_DATA=0;
Delay();
}
dat=dat<<1;
SHT11_SCK=1;
Delay();
SHT11_SCK=0;
}
}
/*********************************
��?�W?SHT11_Answer()�G
��?�\��:??SHT11��??�H?(�b�ĤE???�P��)
***********************************/
void SHT11_Answer()
{
SHT11_SCK=1;
Delay();
while(SHT11_DATA==1);
SHT11_SCK=0;
SHT11_DATA=1;
}
/************************************
��?�W?�GSHT11_Test_Finish()
��?�\��:??SHT11??��??�O�_��?
*************************************/
void SHT11_Test_Finish()
{
while(SHT11_DATA==1);
}
/************************************
��?�W?�GSHT11_Receivebyte()
��?�\��:?SHT11����8bite?�u
*************************************/
uchar SHT11_Receivebyte()
{
uchar i;
uchar dat;
SHT11_SCK=0;
Delay();
for(i=0;i<8;i++)
{
SHT11_SCK=1;
Delay();
dat=dat<<1;
if(SHT11_DATA)
{
dat=dat|0x01;
Delay();
}
else
{
dat=dat&0xfe;
Delay();
}
SHT11_SCK=0;
Delay();
}
SHT11_DATA=1; //?��?�u??
return(dat);
}
/***********************************
��?�W?�GMCU_Answer()
��?�\��:?����VSHT11?�e?���H?
*************************************/
void MCU_Answer()
{
SHT11_SCK=0;
Delay();
SHT11_DATA=0;
Delay();
SHT11_SCK=1;
Delay();
SHT11_SCK=0;
Delay();
SHT11_DATA=1; //?��?�u?? ??���O�D�`���n ���[��??�P?���󤣯�?���C8��
}
/***********************************
��?�W?:SHT11_End()
?����??8byte?�u�Z������CRC��??
************************************/
void SHT11_End()
{
SHT11_DATA=1;
SHT11_SCK=1;
Delay();
SHT11_SCK=0;
Delay();
}
/*************************************
��?�W?�Gvoid SHT11_Write_Register(uchar command ,uchar dat)
��??���G�VSHT11��??�H�s��?�m�\��
command?REG_WRITE 0x06?�H�s��
dat? ?�mSHT11���\�� �i�H?�m??��?�u��?
*/
void SHT11_Write_Register(uchar command ,uchar dat)
{
SHT11_Start();
SHT11_Sendbyte(command);
SHT11_Answer();
SHT11_Sendbyte(dat);
SHT11_Answer();
}
/***************************************
��?�W?�Guchar SHT11_Read_Register(uchar command)
��??���Gcommand?REG_READ  0x07//?�H�s��
��^��???�H�s������
��6?��?�e??���@��?�u�Z?����?��?
?��6?0?���VDD>2.47V ?��6?1?���VDD<2.47V�Y?�q����          ��0���?�e��?�q����v
?��0?1?���?�q��סG8��/?�� 12��?��
?��0?0?���?�q��סG12��?�� 14��?��
�q??0
*******************************************/
uchar SHT11_Read_Register(uchar command)
{
uchar dat;
SHT11_Start();
SHT11_Sendbyte(command);
SHT11_Answer();
dat=SHT11_Receivebyte();
SHT11_End();
return(dat);
}
/***************************************
��?�W?:SHT11_Measure(uchar command,uint time);
��?�\��:?�mSHT11??�\��,�}��^��?��???�G
��??��:command��?�Τ_?�w?��???�O?��??,
time��?�Τ_?�w???�{��������??�A�H�̩w???�G����?
11ms/55ms/210ms ��???8��/12��/14��
****************************************/
uint SHT11_Measure(uchar command,uchar time)
{
uint dat=0;
uchar data_high,data_low;
SHT11_Start();
SHT11_Sendbyte(command);
SHT11_Answer();
Delay_Ms(time);
SHT11_Test_Finish();
data_high=SHT11_Receivebyte();
MCU_Answer();
data_low=SHT11_Receivebyte();
SHT11_End();
dat=(dat|data_high);
dat=(dat<<8)|data_low;
return(dat);
}
/****************************************
��?�W?:Convert_Tempeture12bit(uint dat);
��?�\��:???�쪺?�u?��?��?��?��?�u
��??��:?��??����--T=d1+d2*SOt
��������??d1=-40,d2=0.04
��Τ_12��?�q���
*/
float SHT11_Convert_Tempeture12bit(uint dat)
{
float tempeture1;
tempeture1=-40+0.04*dat;
if(tempeture1>23)
tempeture1=tempeture1+1;
if(tempeture1>55)
tempeture1=tempeture1+1;
if(P37==1)
{
if(tempeture1>=16&&tempeture1<30)
{
P33=1;
P32=0;
}
else
{
P33=0;
P32=1;
}
}
return(tempeture1);
}
/*****************************************
��?�W?:SHT11_Convert_Humidity8bit(uint dat,float temp)
��?�\��:???�쪺?�u?��?��?��?��?�u
��??��:��??��??����-----RHline=C1+C2*SOrh+C3*SOrh*SOrh(???�u��?�ʤ� SOrh??���󱵦��쪺?�u)
-----RHtrue=(tempeture-25)*(t1+t2*SOrh)+RHline           ��������??:C1=-4,C2=0,648,C3=-0.00072
t1=0.01,t2=0.00128
��Τ_8��?�q���
*/
uint SHT11_Convert_Humidity8bit(uint dat,float temp)
{
float RHline,RHtrue;
uint r;
RHline=-4+0.648*dat-0.00072*dat*dat;
RHtrue=(temp-25)*(0.01+0.00128*dat)+RHline;
r=(RHtrue-3)*10+0.5;
if(P37==0)
{
if(r>=400&&r<600)
{
P33=1;
}
else
{
P33=0;
}
if(r>=600)
{
P32=1;
}
else
{
P32=0;
}
}
return(r);
}
#endif
