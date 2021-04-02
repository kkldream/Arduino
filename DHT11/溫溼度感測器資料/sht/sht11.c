   #include <reg52.h>
#include "SHT11.h"
#define uchar unsigned char
#define uint  unsigned int
#define TEM_TEST         0x03//?��??�R�O
#define HUM_TEST          0x05//?��??�R�O
#define REG_READ          0x07//?�H�s��
#define REG_WRITE         0x06//?�H�s��
#define FUNCTION_SET    0x01//?�mSHT11���u�@���?8��/?�� 12��?�� uchar DispData[4] = {0, 1, 2, 3};
code uchar DispSegmentP0[10]={0x3f,0x06,0x1b,0x0f,0x26,0x2d,0x3d,0x07,0x3f,0x2f}; code uchar DispSegmentP2[10]={0x00,0x00,0x22,0x22,0x22,0x22,0x22,0x00,0x22,0x22};                           //  0    1   2    3    4   5   6    7   8    9  % code uchar DispCtrl[4] = {0xef, 0xdF, 0xbf ,0x7F};
sbit P16=P1^6;
sbit P26=P2^6;
sbit P34=P3^4;
void Temp_delay(unsigned int j)
{
uchar i;
for(i=100;i>0;i--)
{
for(j;j>0;j--);
}
}
void Show(uchar  *Buffer)
{
uchar i;
for    (i=0; i<4; i++)
{
P1 = DispCtrl[i];
P0 = DispSegmentP0[*Buffer];
P2 = DispSegmentP2[*Buffer];
if(P16==0)
P26=1;
Temp_delay(2);
Buffer++;
}
}
void  DispConvert(uchar *DispAddr, uint  Temp2Con)
{
DispAddr[0] = Temp2Con/1000;
DispAddr[1] = (Temp2Con - DispAddr[0]*1000)/100;
DispAddr[2] = (Temp2Con - DispAddr[0]*1000 - DispAddr[1]*100)/10;
DispAddr[3] = Temp2Con -  DispAddr[0]*1000 - DispAddr[1]*100 - DispAddr[2]*10;
}
void main()
{
uint temp;
uint dat;
uint Hum;
//float f;
while(1)
{
Show(DispData);
SHT11_Write_Register(REG_WRITE,FUNCTION_SET);    temp=SHT11_Measure(TEM_TEST,0x37);
temp=SHT11_Convert_Tempeture12bit(temp);        dat=SHT11_Measure(HUM_TEST,0x0b);
Hum=SHT11_Convert_Humidity8bit(dat,temp);    if(P34==0)
{
P36=1;
P37=0;
DispConvert(DispData,Hum);
}
else
{
P36=0;
P37=1;
DispConvert(DispData,temp*10);
}
}
}
