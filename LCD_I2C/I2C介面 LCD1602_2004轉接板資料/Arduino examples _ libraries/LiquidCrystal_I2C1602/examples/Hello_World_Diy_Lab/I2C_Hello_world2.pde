// example: I2C_Hello_world2.pde
// Compatible with the Arduino IDE 1.0 & later versions

/*-------------- The following description edited by DIY_LAB ---------------
// 範例：I2C_Hello_world2.pde

◆ 實驗目的：
   使用一般型LCD和Arduino連接，最少需要用到Arduino 6個(I/O)pins 即RS，EN，D7，D6，D5，D4。
   因為ArduinoI/O只有20個，若再加上使用其它感測器、SD卡、繼電器等模組顯然I/O就不敷使用，
   於是I2C（IIC）介面因應而生，它只動用到Arduino 2個I/O Pin(A4,A5)，讓你省下4個I/O Pin
   可供其它擴展模組使用.
   
   此範例利用I2C介面模組及其函式庫，讓我們簡化接線及程式撰寫的繁瑣問題，大大提升工作效率，
   本範例在讀到第10秒時，LCD會變暗1秒，在第1~9秒時背光會亮.

◆ 所需模組：I2C模組、Arduino模板 各1
   Arduino請下載 I2C_Hello_word1.pde 範例程式.
   
   ＊注意:
   此範例會用到 LiquidCrystal_I2C library

◆ 模組引腳連接如下：
  【I2C】  與   【Arduino】 接線如下:
    GND    -     GND 
    VCC    -     +5V 
    SDA    -     A4 
    SCL    -     A5

注意：
Arduino Mega，SDA是數字引腳20和SCL為21。
藍色電位器用來調整亮度對比。若看不到任何文字顯示請調整此電位器。

範例Demo: http://youtu.be/jP3Uo0JylYs
---------------------------------------------------------------------------*/


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


//因為不同版本 PCF8574 IC其內部預設I2C定址不同,以下為常用兩種版本,請依 PCF8574 內部I2C定址或自行設定其他位址
//PCF8574T 版本:對I2C上A0~A2位址設定(0x20~0x27)，默認地址為0x27
//PCF8574AT 版本:對I2C上A0~A2位址設定(0x30~0x37)，默認地址為0x3F

//對I2C上A0~A2位址設定(0x20~0x27)或(0x30~0x37)，一塊Arduino最多可接8個I2C介面，默認地址為0x27 或 0x3F
LiquidCrystal_I2C lcd(0x27,16,2);  //建立LiquidCrystal_I2C物件,並設定LCD位址=0x27 或 0x3F

int time;


void setup() {

  lcd.init();                      //初始化LCD 
  lcd.backlight(); //打開LCD背光源;否則LCD背光源是暗的
  
  //一開始讓LCD顯示之訊息
  lcd.print("== Welcome to =="); //一開始從(第0行,第0列) 即 第1行第1列開始顯"== Welcome to =="
  lcd.setCursor(4, 1);           //lcd.setCursor(x,y)移動游標,即游標移到(第5(=4+1)個字,第2(=1+1)列)
  lcd.print("DIY_LAB");          //顯示"DIY_LAB"
}


void loop() {

  //移動游標到(第16個字,第2列)
  lcd.setCursor(15, 1);        

  time=(1+(millis()/1000))%10; //秒數取10之餘數(0~9),第10秒餘數=0
  //顯示秒數
  lcd.print(time);             
  
  //背光開或關
  if (time==0) {
    lcd.noBacklight(); //第10秒時,背光關閉
  }else if(time==1){
    lcd.backlight();   //第1~9秒,背光打開
  }
}