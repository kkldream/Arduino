 #ifndef __SHT11_H__
#define __SHT11_H__
/*************************
SHT11相?命令
**************************/
#define uchar unsigned char
#define uint  unsigned int
#define TEM_TEST         0x03//?度??命令
#define HUM_TEST          0x05//?度??命令
#define REG_READ          0x07//?寄存器
#define REG_WRITE         0x06//?寄存器
#define FUNCTION_SET    0x01//?置SHT11的工作精度?8位/?度 12位?度 /**************************

sbit SHT11_DATA=P3^1;
sbit SHT11_SCK=P3^0;
sbit P33=P3^3;
sbit P32=P3^2;
sbit P36=P3^6;
sbit P37=P3^7;
uchar flag_tempeture=0;  //?示?度位置的?志
uchar flag_humidity=0;     //?示?度位置的?志
uchar code str1[]={ 0x10,0x06,0x09,0x08,0x08,0x09,0x06,0x00};//?度??
uchar code str6_sht11[]="%RH   ";
uchar code str4_sht11[]="humi=";
uchar code str2_sht11[]="temp=";
uchar code str7_sht11[]="    ";//清除?不要的?示



void Delay_Ms(uint ms)    // ms延?函? (AT89C51 @ 11.0592MHz)
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
函?名?SHT11_Answer()：
函?功能:??SHT11的??信?(在第九???周期)
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
函?名?：SHT11_Test_Finish()
函?功能:??SHT11??度??是否完?
*************************************/
void SHT11_Test_Finish()
{
while(SHT11_DATA==1);
}
/************************************
函?名?：SHT11_Receivebyte()
函?功能:?SHT11接收8bite?据
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
SHT11_DATA=1; //?放?据??
return(dat);
}
/***********************************
函?名?：MCU_Answer()
函?功能:?片机向SHT11?送?答信?
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
SHT11_DATA=1; //?放?据?? ??指令非常重要 不加的??致?片机不能?取低8位
}
/***********************************
函?名?:SHT11_End()
?接收??8byte?据后部接收CRC校??
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
函?名?：void SHT11_Write_Register(uchar command ,uchar dat)
函??明：向SHT11的??寄存器?置功能
command?REG_WRITE 0x06?寄存器
dat? ?置SHT11的功能 可以?置??的?据位?
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
函?名?：uchar SHT11_Read_Register(uchar command)
函??明：command?REG_READ  0x07//?寄存器
返回值???寄存器的值
位6?示?前??完一次?据后?源供?情?
?位6?0?表明VDD>2.47V ?位6?1?表明VDD<2.47V即?量不足          位0表明?前的?量分辨率
?位0?1?表明?量精度：8位/?度 12位?度
?位0?0?表明?量精度：12位?度 14位?度
默??0
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
函?名?:SHT11_Measure(uchar command,uint time);
函?功能:?置SHT11??功能,并返回相?的???果
函??明:command形?用于?定?度???是?度??,
time形?用于?定???程中的等待??，以确定???果的位?
11ms/55ms/210ms 分???8位/12位/14位
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
函?名?:Convert_Tempeture12bit(uint dat);
函?功能:???到的?据?化?相?的?度?据
函??明:?度??公式--T=d1+d2*SOt
公式中的??d1=-40,d2=0.04
适用于12位?量精度
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
函?名?:SHT11_Convert_Humidity8bit(uint dat,float temp)
函?功能:???到的?据?化?相?的?度?据
函??明:相??度??公式-----RHline=C1+C2*SOrh+C3*SOrh*SOrh(???据的?性化 SOrh??片机接收到的?据)
-----RHtrue=(tempeture-25)*(t1+t2*SOrh)+RHline           公式中的??:C1=-4,C2=0,648,C3=-0.00072
t1=0.01,t2=0.00128
适用于8位?量精度
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
