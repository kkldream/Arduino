// example: DIY_LAB_I2C_Auto_Set_Addr.pde (自動設定I2C位址並顯示I2C位址等訊息)
// Designed & programed by DIY_LAB
// Compatible with the Arduino IDE 1.0 & later versions

/*-------------- The following description edited by DIY_LAB ---------------
// 範例：DIY_LAB_I2C_Auto_Set_Addr.pde

◆ 實驗目的：
   使用一般型LCD和Arduino連接，最少需要用到Arduino 6個(I/O)pins 即RS，EN，D7，D6，D5，D4。
   因為ArduinoI/O只有20個，若再加上使用其它感測器、SD卡、繼電器等模組顯然I/O就不敷使用，
   於是I2C（IIC）介面因應而生，它只動用到Arduino 2個I/O Pin(A4,A5)，讓你省下4個I/O Pin
   可供其它擴展模組使用.
   
   此範例利用I2C介面模組及其函式庫，讓我們簡化接線及程式撰寫的繁瑣問題，大大提升工作效率，
   本範例只簡單秀出簡單訊息及I2C地址.
   
   鑒於不同版本 PCF8574 IC 其內部預設I2C定址不同(常用為0x27,0x3F)使得設計者常得更換不同位址,
   本範例利用 i2c_scanner函式,自動找出I2C裝置的位址,並以此位址直接建構物件,並利用內建函式來顯示訊息.
   
◆ 所需模組：I2C模組、Arduino模板 各1
   Arduino請下載 DIY_LAB_I2C_Auto_Set_Addr.pde 範例程式.
   
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
藍色電位器用來調整亮度對比。若看不到任何文字顯示，請調整此電位器。
---------------------------------------------------------------------------*/

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


//因為不同版本 PCF8574 IC其內部預設I2C定址不同,以下為常用兩種版本,請依 PCF8574 內部I2C定址或自行設定其他位址
//PCF8574T 版本:對I2C上A0~A2位址設定(0x20~0x27)，默認地址為0x27
//PCF8574AT 版本:對I2C上A0~A2位址設定(0x30~0x37)，默認地址為0x3F
//LiquidCrystal_I2C lcd(0x27,16,2);  //建立LiquidCrystal_I2C物件,並設定LCD位址=0x27
//LiquidCrystal_I2C lcd(0x3f,16,2);  //建立LiquidCrystal_I2C物件,並設定LCD位址=0x3f



byte error, address, i2c_addr;
int nDevices;

void setup()
{
  Wire.begin();

  Serial.begin(9600);
  
  Serial.println("\nI2C Scanner");
  
  // 找出I2C位址
  i2c_scanner();
  Serial.println(i2c_addr,HEX);
   
  //建立LiquidCrystal_I2C物件,並設定LCD位址   
  LiquidCrystal_I2C lcd(i2c_addr,16,2);
  lcd.init();                       //初始化LCD 
  lcd.backlight();                  //打開LCD背光源;否則LCD背光源是暗的
  
  //一開始讓LCD顯示之訊息
  lcd.print("== Welcome to ==");    // 一開始從(第0行,第0列) 即 第1行第1列開始顯示文字
  lcd.setCursor(3, 1);              // lcd.setCursor(x,y)移動游標,即游標移到(第4(=3+1)個字,第2(=1+1)列)
  lcd.print("DIY_LAB ox");          // 顯示"DIY_LAB"
  if (i2c_addr<16) lcd.print("0");  // <16 補 0 (好看)
  lcd.print(i2c_addr,HEX);          // 顯示目前I2C位址
  
}




void loop()
{
   //--non--
}




//******************
//  I2C 地址掃描
//******************
void i2c_scanner() {
  
  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
	// 利用Write.endTransmisstion傳回值來判斷I2C位址
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      i2c_addr = address;
      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknow error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

}