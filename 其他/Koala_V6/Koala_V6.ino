/* ====== Koala_V6.ino 非常跡車 ========
   0xF0:遙控車(藍牙)
   0xF1:智走車(超音波)
   0xF2:循跡車(紅外線)
   新尾燈
   2016/09/24  V6.0 @ yiher
*/
//============================================
/* =========== 線路連接 ============
    超音波模組Echo=A1,Trig=A0
    轉頭伺服機SERVO=A2
    紅外線模組IL=A5,IM=A4,IR=A3
    大燈LampM=10,左方向燈LampL=9,右方向燈LampR=8
    右馬達M12=7,M11=6; 左馬達M22=4,M21=5
    蜂鳴器BZ=3; 尾燈LED=2
    藍牙=1,0
    ISP port:RST,SCK=13,MISO=12,MOSI=11
*/
//===========================================
/* KTM-600多功能開發平台的接線(模擬)
   4腳P24A-1直流馬達1
   5腳P24A-2
   6腳P24A-3直流馬達2
   7腳P24A-4
   8腳接P22-10(右轉燈)
   9腳接P22-1(左轉燈)
   10腳接P23(大燈)
   3腳接KT-BZLED之DIN
   2腳接KT-BZLED之BZ-
   A2腳接P11-1(指撥開關)，OFF時藍牙控制
   KT-BZLED之Vcc連接+5V
   KT-BZLED之GND連接GND
   === 燒錄時，以下不要連接：完成燒錄後才連接 ===
   0腳接P5A-1(藍牙之TxD)
   1腳接P5A-2(藍牙之RxD)
*/
//=======================================================
//==================== 定義接腳 =====================
// 燒錄埠
// ISP Port 1:RST, 19=SCK=13, 18=MISO=12, 17=MOSI=11
//=== 車燈 ==========================================
#define LampM 10 // 16 大燈(低態動作)
#define LampL 9  // 15 左方向燈(低態動作)
#define LampR 8  // 14 右方向燈(低態動作)

//=== 紅外線感測器(黑膠帶高態)循跡感測 ================
const int IR[] = {A5, A4, A3};
// IR[0]=A5 ：左紅外線感測器
// IR[1]=A4 ：中紅外線感測器
// IR[2]=A3 ：右紅外線感測器
bool track = false; //true:黑底白軌跡(W),false:白底黑軌跡(B)

//=== 超音波感測器 ==========================
#define Echo A1      // 24 超音波感測器
#define Trig A0      // 23 超音波感測器
#define obstacle 6  // 最近障礙物距離
float distance;
int distanceL, distanceR, distanceF;

//=== 轉頭伺服機 ==========================
#include <Servo.h>
#define SERVO A2
Servo myServo;
#define dirF 88
#define dirL 10
#define dirR 170

//=== 馬達 ==================================
#define M12 7        // 13  右馬達
#define M11 6        // 12~ 右馬達
#define M21 5        // 11~ 左馬達
#define M22 4        // 6  左馬達

//=== 後端模組 ========================
#define BZ 3        // 5 蜂鳴器(低態驅動)V3.1適用
#define LED 2       // 4  RGB LED8 V3.1適用

//=====================================
byte SW = 0xF0;      // 0xF0(藍牙遙控), 0xF1(智走車), 0xF2(循跡車)
byte left = 0;       //0不亮，1閃燈
byte right = 0;      //0不亮，1閃燈
byte trace, ir, dir = 0, FS; //宣告變數dir=0前進、1後退

//======= 尾燈 ===============================
#include <Adafruit_NeoPixel.h>  //包含Adafruit_NeoPixel標頭檔
#define maxLux 100
#define LED_CNTS 8
int Bright = 15, x = 0; //宣告變數
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_CNTS, LED, NEO_GRB + NEO_KHZ800);

const byte left_1[4] = {0, 1, 5, 4}; //雙繞圈-左
const byte right_1[4] = {3, 2, 6, 7}; //雙繞圈-右

//====== led花樣陣列 ========================
const byte extend_1[2] = {1, 0}; //左上
const byte extend_2[2] = {5, 4}; //左下
const byte extend_3[2] = {2, 3}; //右上
const byte extend_4[2] = {6, 7}; //右下

//======= 演奏 ==============================
#define  speed 48    //定義歌曲速度(原為50)
//宣告基本音階陣列
const int tone0[] = {494, 523, 587, 659, 698, 784, 880, 988, 1046, 1175, 1318};
//宣告望春風音階陣列
const int song0[] = { 2, 2, 3, 5,  6, 5, 6, 7,  9, 7, 7, 6, 5,  6,
                      7, 9, 9, 7, 9,  5, 6, 6,  2, 7, 7, 6, 5,  5,
                      6, 6, 7, 6, 5,  3, 2, 3, 5,  3, 5, 6, 7,  9,
                      9, 9, 10, 9, 7,  7, 6, 5, 3,  2, 7, 7, 6, 5,  5, 100
                    }; //100為結束符號
//宣告望春風節拍陣列
const int beat0[] = { 12, 4, 8, 8,  8, 4, 4, 16,  12, 4, 4, 4, 8,  32,
                      12, 4, 8, 4, 4,  12, 4, 16,  12, 4, 8, 4, 4,  32,
                      12, 4, 8, 4, 4,  8, 4, 4, 16,  12, 8, 8, 8,  32,
                      12, 4, 8, 4, 4,  8, 4, 4, 16,  12, 4, 8, 4, 4,  32
                    };

int CNTs = 0;
#define test 200

//========== 初始設定部分 ============
void setup() {
  myServo.attach(SERVO);   //伺服機連線
  for (int i = dirL; i <= dirR; i += 5)
  { myServo.write(i);
    delay(20);
  }
  myServo.write(dirF);//伺服機轉向正前方

  pinMode(Trig, OUTPUT); digitalWrite(Trig, 0);

  strip.begin();           //啟用Adafruit_NeoPixel
  strip.show();            //重置LED，開始顯示

  Serial.begin(9600);       //啟用串列埠

  pinMode(BZ, OUTPUT); digitalWrite(BZ, 1);
  pinMode(LED, OUTPUT); digitalWrite(LED, 0);
  pinMode(M11, OUTPUT); digitalWrite(M11, 0);  //高態動作
  pinMode(M12, OUTPUT); digitalWrite(M12, 0);  //高態動作
  pinMode(M21, OUTPUT); digitalWrite(M21, 0);  //高態動作
  pinMode(M22, OUTPUT); digitalWrite(M22, 0);  //高態動作
  pinMode(LampM, OUTPUT); digitalWrite(LampM, 1); //低態動作
  pinMode(LampL, OUTPUT); digitalWrite(LampL, 1); //低態動作
  pinMode(LampR, OUTPUT); digitalWrite(LampR, 1); //低態動作
  for (int i = 0; i < 3; i++)  pinMode(IR[i], INPUT); //設定IR為輸入埠

  //彩虹輪動
  rainbowCycle(2, 5);

  //播放望春風
  //SONG();
}

//========== 主程式部分 ============
void loop() {
  switch (SW)   //SW變動才會返回
  { case  0xF0: //藍牙遙控車
      offLamp();//關方向燈
      beep1k(2);
      BTCar();
      break;
    case  0xF1: //智走車
      offLamp();//關方向燈
      beep1k(2);
      iCar();
      break;
    case  0xF2: //IR循跡車
      offLamp();//關方向燈
      beep1k(2);
      Tracer();
      break;
  }
}
//==============================================

//========== BTCar() 藍牙遙控車函數 ============
void BTCar(void) {
  // 遠光燈：H(high beam)
  // 近光燈：D(dipped beam)
  // 關燈：O(Off)  mainBeam
  // 左轉燈：l(left Lamp)、右轉燈：r(right Lamp)、關閉方向燈：n
  // 前進：f、後退：b、停車：s、左轉：L、右轉：R
  // 喇叭：P
  while (SW == 0xF0)
  { if (Serial.available())              //若有指令傳入
    { byte instruction = Serial.read();  //讀取指令
      switch (instruction)              //根據指令執行下列事項
      { case  0xF1 :  //智走車
          SW = 0xF1; stopCar(); myServo.write(dirF);
          break;
        case  0xF2 :  //循跡車
          SW = 0xF2; stopCar(); myServo.write(dirF);
          break;
        case  'H' :   //遠光燈(High beam)
          digitalWrite(LampM, 0); //大燈全亮
          break;
        case  'D':    //近光燈(Dipped beam)
          analogWrite(LampM, 160); //大燈半亮
          break;
        case  'O':    //關燈
          digitalWrite(LampM, 1); //關閉大燈
          break;
        case  'f':    //前進(高耗電，若電力不足可能會不動作)
          fullSpeed();//前速前進
          left = 0; right = 0; //關方向燈
          dir = 0;
          FS_LED(); //尾燈
          rainbowCycle(1, 5);
          break;
        case  'b':    //中速後退
          middleBack(200);
          left = 1; right = 1; //閃方向燈
          beep1k(1);
          dir = 1;
          //double_C1(strip.Color(127,0,0),50); //紅色
          //double_C1(strip.Color(127,0,0),50); //紅色
          break;
        case  's':    //停車
          stopCar();
          left = 0; right = 0; //關方向燈
          dir = 0;
          //彩虹輪動
          rainbowCycle(1, 10);
          break;
        case  'L':    //左轉
          if (dir == 0)
          { analogWrite(M11, 200); //右快
            digitalWrite(M12, 0);
            analogWrite(M21, 120); //左慢
            digitalWrite(M22, 0);
          }
          else
          { analogWrite(M11, 50);
            digitalWrite(M12, 1);
            analogWrite(M21, 100);
            digitalWrite(M22, 1);
          }
          left = 1; right = 0; //左轉燈
          break;
        case  'R':      //右轉
          if (dir == 0)
          { analogWrite(M11, 120); //右慢
            digitalWrite(M12, 0);
            analogWrite(M21, 200); //左快
            digitalWrite(M22, 0);
          }
          else
          { analogWrite(M11, 100);
            digitalWrite(M12, 1);
            analogWrite(M21, 50);
            digitalWrite(M22, 1);
          }
          left = 0; right = 1; //右轉燈
          break;
        case  'x':      //順時針轉
          analogWrite(M11, 55); //右反轉
          digitalWrite(M12, 1);
          analogWrite(M21, 200); //左正轉
          digitalWrite(M22, 0);
          rainbowCycle(2, 5);
          stopCar();
          break;
        case  'y':      //逆時針轉
          analogWrite(M11, 200); //右正轉
          digitalWrite(M12, 0);
          analogWrite(M21, 55); //左反轉
          digitalWrite(M22, 1);
          rainbowCycle(2, 5);
          stopCar();
          break;
        case  'l':      //左轉燈
          left = 1; right = 0; //左轉燈
          break;
        case  'r':      //右轉燈
          left = 0; right = 1; //右轉燈
          break;
        case  'n':      //關閉方向燈
          left = 0; right = 0;
          for (int i = 0; i < 8; i++) strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
          strip.show();   //顯示LED
          break;
        case  'P':      //喇叭
          beep(2);
          break;
        case  'S':      //演奏
          rainbowCycle(1, 10);
          SONG();
          break;
      }
    }
    //================================================
    if (!right && !left) ; //double_C2(strip.Color(0,0,127),50);//rainbowCycle(1, 10);
    else
    { // 方向燈
      digitalWrite(LampL, !left);
      digitalWrite(LampR, !right);
      if (left && right)
      {
        beep0();
        t_Flash(75);
      }
      else if (left) t_Left(75);
      else if (right) t_Right(75);

      digitalWrite(LampL, 1);
      digitalWrite(LampR, 1);
      delay(75);
    }

  }
  //--------------------------------------
}

//========== iCar() 智走車函數 ============
void iCar(void) {
  boolean Enable = false;
  while (SW == 0xF1)
  { if (Serial.available())              //若有指令傳入
    { byte instruction = Serial.read();  //讀取指令
      switch (instruction)              //根據指令執行下列事項
      { case  0xF2 :  //循跡車
          SW = 0xF2; stopCar(); myServo.write(dirF);
          break;
        case  0xF0 :  //遙控車
          SW = 0xF0; stopCar(); myServo.write(dirF);
          break;
        case  's':    //手動停車
          Enable = false;
          stopCar();
          beep1k(2);
          rainbowCycle(1, 10); //彩虹輪動
          break;      //跳出
        case  'f':    //手動前進(開車)，並進入智走模式
          Enable = true;
          beep1k(2);
          double_C1(strip.Color(0, 127, 0), 50); //綠色
          break;      //跳出
        case  'H' :   //遠光燈(High beam)
          digitalWrite(LampM, 0); //大燈全亮
          break;      //跳出
        case  'D':    //近光燈(Dipped beam)
          analogWrite(LampM, 160); //大燈半亮
          break;      //跳出
        case  'O':    //關燈
          digitalWrite(LampM, 1); //關閉大燈
          break;      //跳出
        case  'P':      //喇叭
          beep(2);
          break;      //跳出
          /*         case  'S':      //演奏
                     rainbowCycle(1, 10);
                     SONG();
                     break;      //跳出
          */
      }
    }
  }
  //======= 智走車 ========================================
  if (Enable)
  { measure(dirF);
    distanceF = int(distance);
    if (distanceF > 30)
    {
      fullSpeed();
      delay(300);
    }
    if (distanceF > 20 && distanceF < 30) stopCar();
    if (distanceF < 10)
    {
      fullBack();
      delay(300);
    }
  }

  else            //與障礙物距離小於obstacle(5公分)
  { stopCar();
    beep(1);
    measureRL();  //量測左右障礙物(distanceR, distanceL)
    beep(1);
    //===== 進入窄巷 =====
    if (distanceR < obstacle && distanceL < obstacle)
    { //逃脫術
      middleBack(200);//速度200
      delay(200);//後退0.2秒
      stopCar();      //停車
      measureRL();    //量測左右障礙物(distanceR, distanceL)
      while (distanceR < obstacle && distanceL < obstacle)
      { middleBack(200);//速度200
        delay(200);//後退0.2秒
        stopCar();      //停車
        measureRL();    //量測左右障礙物(distanceR, distanceL)
      }
      if (distanceR > distanceL)  turnCW(0, 200); //順時鐘轉0.2秒
      else  turnCW(1, 200); //逆時鐘轉0.2秒
    }
    //===== 右邊無障礙物 =====
    else if (distanceR > distanceL)
    { if (distanceR > 2 * obstacle ) //右轉
      { turnCW(0, 200);
        turnRL(0, 300);     //右轉0.3秒
        middleSpeed(160); //中速前進
      }
      else //順時鐘轉
      { turnCW(0, 500);  //順時鐘轉0.5秒
      }
    }
    //===== 左邊無障礙物 =====
    else if (distanceR < distanceL)
    { if (distanceL > 2 * obstacle ) //左轉
      { turnCW(1, 200);
        turnRL(1, 300);     //左轉0.3秒
        middleSpeed(160); //中速前進
      }
      else //逆時鐘轉
      { turnCW(1, 500);  //逆時鐘轉0.5秒
      }
    }
  }
}
//--------------------------------------

//========== Tracer() 循跡車函數 ============
void Tracer(void) {
  // 黑(軌跡)=1、白(脫軌)=0
  // 000(白白白)：未在軌道上，停
  // 010(白黑白)：直走，全速前進
  // 110(黑黑白)：微偏左，左快、右慢
  // 100(黑白白)：偏左，左最快、右慢
  // 011(白黑黑)：微偏右，左慢、右快
  // 001(白白黑)：偏右，左慢、右最快
  // 111：交叉
  // 101：錯誤，停
  // B：白底黑軌道(false)
  // W：黑底白軌道(true)

  while (SW == 0xF2)
  { if (Serial.available())       //若有指令傳入
    { byte instruction = Serial.read();  //讀取指令
      switch (instruction)    //根據指令執行下列事項
      { case  0xF1 :  //智走車
          SW = 0xF1; stopCar(); myServo.write(dirF);
          break;
        case  0xF0 :  //遙控車
          SW = 0xF0; stopCar(); myServo.write(dirF);
          break;
        case  'H' :   //遠光燈(High beam)
          digitalWrite(LampM, 0); //大燈全亮
          break;      //跳出
        case  'D':    //近光燈(Dipped beam)
          analogWrite(LampM, 160); //大燈半亮
          break;      //跳出
        case  'O':    //關燈
          digitalWrite(LampM, 1); //關閉大燈
          break;      //跳出
        case  'P':      //喇叭
          beep(2);
          break;      //跳出
        case  'B' | 'b':      //白底黑軌道
          track = false;
          break;      //跳出
        case  'W' | 'w':      //黑底白軌道
          track = true;
          break;      //跳出
      }
    }
    //======= 循跡 ========================================
    trace = 0;                    //狀態歸零
    for (int i = 0; i < 3; i++)   //若按鍵被按下
    { ir = digitalRead(IR[i]);    //讀取紅外線感測器
      if (ir) trace |= (1 << (2 - i)); //記錄感測狀態
    }
    digitalWrite(M12, 0);
    digitalWrite(M22, 0);
    if (track) trace ^= 0xFF;
    switch (trace)            //根據trace執行下列動作
    { case 0b100:             //100時(電路板偏左)黑白白
        analogWrite(M11, 128);//右馬達半速
        analogWrite(M21, 250);//左馬達全速
        break;      //跳出
      case 0b110:           //110時(電路板微偏左)黑黑白
        analogWrite(M11, 192);//右馬達3/4速
        analogWrite(M21, 250);//左馬達全速
        break;      //跳出
      case 0b010:           //010時(中間)白黑白
        middleSpeed(200);   //中速前進
        break;      //跳出
      case 0b0011:          //011時(電路板微偏右)白黑黑
        analogWrite(M11, 255);//右馬達全速
        analogWrite(M21, 192);//左馬達3/4速
        break;      //跳出
      case 0b0001:          //001時(電路板偏右)白白黑
        analogWrite(M11, 250);//右馬達全速
        analogWrite(M21, 128);//左馬達半速
        break;      //跳出
      case 0b0111:          //111時(電路板偏右)黑黑黑
        analogWrite(M11, 250);//右馬達全速
        analogWrite(M21, 250);//左馬達全速
        break;      //跳出
      default:              //其他狀態
        // 馬達全關
        stopCar();
        beep1k(3);
        break;      //跳出
    }
    delay(100);     //持續0.1秒
  }
}

//=========================================================================
//==== 量測左右障礙物 =================
void measureRL(void)
{ measure(dirL);            //量測左邊障礙物
  distanceL = int(distance); //記錄距離
  beep1k(1); delay(100);
  measure(dirR);            //量測右邊障礙物
  distanceR = int(distance); //記錄距離
  beep1k(1); delay(100);
  measure(dirF);
}
//==== 超音波量測距離 =================
void measure(byte x)
{ myServo.write(x);          //轉動超音波感測器方向
  //送出10us正脈波
  digitalWrite(Trig, 1);
  delayMicroseconds(20);
  digitalWrite(Trig, 0);
  float pWidth = pulseIn(Echo, HIGH); //量測脈波寬度
  distance = int(pWidth / 58); //換算距離(單位為公分)
}
//==== 關方向燈 =================
void offLamp(void)
{ digitalWrite(LampL, 1); //關閉左轉燈
  digitalWrite(LampR, 1); //關閉右轉燈
  for (int i = 0; i < 8; i++) strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
}

//==== 左右轉 =================
void turnRL(int RL, int delayTime)
{ if (RL == 0) //右轉
  { analogWrite(M11, 100); //右馬達轉慢
    digitalWrite(M12, 0);
    analogWrite(M21, 200); //左馬達轉快
    digitalWrite(M22, 0);
  }
  else      //左轉
  { analogWrite(M11, 200); //右馬達轉快
    digitalWrite(M12, 0);
    analogWrite(M21, 100); //左馬達轉慢
    digitalWrite(M22, 1);
  }
  delay(delayTime);
  stopCar();
}
//==== 順/逆時針轉 =================
void turnCW(int cw, int delayTime)
{ if (cw == 0) //順時針轉cw=0
  { analogWrite(M11, 55); //右反轉
    digitalWrite(M12, 1);
    analogWrite(M21, 200); //左正轉
    digitalWrite(M22, 0);
  }
  else      //逆時針轉cw=1
  { analogWrite(M11, 200); //右正轉
    digitalWrite(M12, 0);
    analogWrite(M21, 55); //左反轉
    digitalWrite(M22, 1);
  }
  delay(delayTime);
  stopCar();
}
//==== 高速後退 =================
void fullBack(void)
{ analogWrite(M11, 240);
  digitalWrite(M12, 1);
  analogWrite(M21, 240);
  digitalWrite(M22, 1);
}
//==== 中速後退 =================
void middleBack(int sp)
{ analogWrite(M11, 255 - sp);
  digitalWrite(M12, 1);
  analogWrite(M21, 255 - sp);
  digitalWrite(M22, 1);
}
//==== 全速前進 =================
void fullSpeed(void)
{ digitalWrite(M11, 1);
  digitalWrite(M12, 0);
  digitalWrite(M21, 1);
  digitalWrite(M22, 0);
}
//==== 中速前進 =================
void middleSpeed(int sp)
{ analogWrite(M11, sp);
  digitalWrite(M12, 0);
  analogWrite(M21, sp);
  digitalWrite(M22, 0);
}
//==== 停車 =====================
void stopCar(void)
{ digitalWrite(M11, 0);
  digitalWrite(M12, 0);
  digitalWrite(M21, 0);
  digitalWrite(M22, 0);
}
//==== 1K嗶聲 ==================
void beep1k(int x)
{ for (int i = 0; i < x; i++)
  { tone(BZ, 1000); delay(100);
    noTone(BZ); delay(100);
  }
}
//==== 1K嗶聲 ==================
void beep0(void)
{ tone(BZ, 1000); delay(100); noTone(BZ);
}
//==== 按喇叭 ==================
void beep(int x)
{ for (int i = 0; i < x; i++)
  { tone(BZ, 440); delay(300);
    noTone(BZ); delay(100);
  }
}
//======== 播放歌曲 ============
void SONG()
{ int x = 0;  //宣告取音指標變數
  bool playSONG = true;
  while (song0[x] != 100 && playSONG) //若非結束符號，則執行迴圈
  { int duration = speed * beat0[x];  //計算節拍
    tone(BZ, tone0[song0[x]], duration); //播音
    delay(duration * 1.3);    //暫停
    x++;          //下一個音
    playSONG = readBT_S();
  }
  noTone(BZ);  //靜音
}
//===== 調整RGB LED亮度 =======
void brightness()
{ strip.setBrightness(Bright);
  Bright += 20;
  if (Bright > maxLux) Bright = 15;
}
//====== 彩虹輪動 =============
// counts值為輪動圈數
// 改變waiting值，可調整顯示的時間長度
void rainbowCycle(uint8_t counts, uint8_t waiting)
{ for (int i = 0; i < counts; i++) // 彩虹輪動counts圈
    for (int j = 0; j < 256; j++) // 每圈256個顏色變化
    { //brightness();   // 調整亮度
      for (int k = 0; k < strip.numPixels(); k++) //設定每個LED的顏色
        strip.setPixelColor(k, Wheel(((k * 256 / strip.numPixels()) + j) & 255));
      strip.show(); //顯示LED
      delay(waiting); //持續點亮waiting時間
    }
}
//====== 產生輪動色階函數 ===========
// 輸入0~255(輪位)，以取得其色階()
// 顏色變遷由紅藍綠紅
uint32_t Wheel(byte WheelPos)
{ WheelPos = 255 - WheelPos;
  if (WheelPos < 85)    // 若輪位小於85
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  // 填入顏色：R=255減去3倍輪位，G=0，B= 3倍輪位
  // 輪位=0時：R=255，G=0，B= 0，即紅色
  // 輪位=84時：R=3，G=0，B= 252，即藍色
  // 也就是由紅到藍
  else if (WheelPos < 170) // 若輪位在85~170之間
  { WheelPos -= 85;   // 輪位減去85，往前移動
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    // 填入顏色：R=0，G= 3倍輪位，B= 255減去3倍輪位
    // 輪位=0時(85)：R=0，G=0，B= 255，即藍色
    // 輪位=84時(169)：R=0，G=252，B=3，即綠色
    // 也就是由藍到綠
  }
  else        // 若輪位大於於170
  { WheelPos -= 170;  // 輪位減去170，往前移動
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    // 填入顏色：R=3倍輪位，G= 255減去3倍輪位，B= 0
    // 輪位=0時(170)：R=0，G=255，B=0，即綠色
    // 輪位=84時(255)：R=252，G=3，B=0，即紅色
    // 也就是由綠到紅
  }
}
//======= 雙圈-正轉 ==========
void double_C1(uint32_t c24, uint8_t waiting)
{ for (int i = 0; i < 4; i++)
  { for (int j = 0; j < 8; j++)
      strip.setPixelColor(j, strip.Color(0, 0, 0)); //清除LED的顏色
    strip.setPixelColor(left_1[i], c24); //設定LED的顏色
    strip.setPixelColor(right_1[i], c24); //設定LED的顏色
    strip.show();   //顯示LED
    delay(waiting);   //持續點亮waiting時間
  }
}
//======= 雙圈-反轉 ==========
void double_C2(uint32_t c24, uint8_t waiting)
{ for (int i = 0; i < 4; i++)
  { for (int j = 0; j < 8; j++)
      strip.setPixelColor(j, strip.Color(0, 0, 0)); //清除LED的顏色
    strip.setPixelColor(left_1[3 - i], c24); //設定LED的顏色
    strip.setPixelColor(right_1[3 - i], c24); //設定LED的顏色
    strip.show();   //顯示LED
    delay(waiting);   //持續點亮waiting時間
  }
}
//======= 展開 ==========
void E1(uint32_t c24, uint8_t waiting)
{ for (int i = 0; i < 2; i++)
  { strip.setPixelColor(extend_1[i], c24); //設定LED的顏色
    strip.setPixelColor(extend_2[i], c24); //設定LED的顏色
    strip.setPixelColor(extend_3[i], c24); //設定LED的顏色
    strip.setPixelColor(extend_4[i], c24); //設定LED的顏色
    strip.show();   //顯示LED
    delay(waiting);   //持續點亮waiting時間
  }
}
//======= 往內 ==========
void E2(uint32_t c24, uint8_t waiting)
{ for (int i = 0; i < 2; i++)
  { strip.setPixelColor(extend_1[1 - i], c24); //設定LED的顏色
    strip.setPixelColor(extend_2[1 - i], c24); //設定LED的顏色
    strip.setPixelColor(extend_3[1 - i], c24); //設定LED的顏色
    strip.setPixelColor(extend_4[1 - i], c24); //設定LED的顏色
    strip.show();   //顯示LED
    delay(waiting);   //持續點亮waiting時間
  }
}
//======= 左轉尾燈 ==========(黃)
void t_Left(uint8_t waiting)
{ for (int i = 0; i < 8; i++) strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
  for (int i = 0; i < 4; i++)
  { strip.setPixelColor((3 - i), strip.Color(255, 255, 0)); //設定LED的顏色
    strip.show();   //顯示LED
    delay(waiting);   //持續點亮waiting時間
    strip.setPixelColor((3 - i), strip.Color(0, 0, 0)); //關閉LED
  }
  //for(int i=0;i<8;i++) strip.setPixelColor(i, strip.Color(0,0,0)); //關閉LED
}
//======= 右轉尾燈 ==========(黃)
void t_Right(uint8_t waiting)
{ for (int i = 0; i < 8; i++) strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
  for (int i = 4; i < 8; i++)
  { strip.setPixelColor(i, strip.Color(255, 255, 0)); //設定LED的顏色
    strip.show();   //顯示LED
    delay(waiting);   //持續點亮waiting時間
    strip.setPixelColor((3 - i), strip.Color(0, 0, 0)); //關閉LED
  }
}
//======= 閃爍 ==========(紅)
void t_Flash(uint8_t waiting)
{ for (int i = 0; i < 8; i++) strip.setPixelColor(i, strip.Color(255, 0, 0)); //亮紅LED
  strip.show();   //顯示LED
  delay(waiting);   //持續點亮waiting時間
  for (int i = 0; i < 8; i++) strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
  strip.show();   //顯示LED
  delay(waiting);   //持續點亮waiting時間
}
//======= 前進 ==========
void FS_LED(void)
{ for (int i = 0; i < 8; i++) strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
  //紅
  for (int i = 0; i < 4; i++)
  { strip.setPixelColor(i, strip.Color(255, 0, 0)); //設定LED的顏色
    strip.setPixelColor(7 - i, strip.Color(255, 0, 0)); //設定LED的顏色
    strip.show();   //顯示LED
    delay(50);   //持續點亮waiting時間
    strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
    strip.setPixelColor(7 - i, strip.Color(0, 0, 0)); //關閉LED
  }
  //綠
  for (int i = 0; i < 4; i++)
  { strip.setPixelColor(i, strip.Color(0, 255, 0)); //設定LED的顏色
    strip.setPixelColor(7 - i, strip.Color(0, 255, 0)); //設定LED的顏色
    strip.show();   //顯示LED
    delay(50);   //持續點亮waiting時間
    strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
    strip.setPixelColor(7 - i, strip.Color(0, 0, 0)); //關閉LED
  }
  for (int i = 0; i < 4; i++)
  { strip.setPixelColor(i, strip.Color(0, 0, 255)); //設定LED的顏色
    strip.setPixelColor(7 - i, strip.Color(0, 0, 255)); //設定LED的顏色
    strip.show();   //顯示LED
    delay(50);   //持續點亮waiting時間
    strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
    strip.setPixelColor(7 - i, strip.Color(0, 0, 0)); //關閉LED
  }
  //白
  for (int i = 0; i < 4; i++)
  { strip.setPixelColor(i, strip.Color(255, 255, 255)); //設定LED的顏色
    strip.setPixelColor(7 - i, strip.Color(255, 255, 255)); //設定LED的顏色
    strip.show();   //顯示LED
    delay(50);   //持續點亮waiting時間
    strip.setPixelColor(i, strip.Color(0, 0, 0)); //關閉LED
    strip.setPixelColor(7 - i, strip.Color(0, 0, 0)); //關閉LED
  }
}
//======= 讀取藍牙-1 ======================
bool readBT_S(void)
{ if (Serial.available())              //若有指令傳入
  { byte instruction = Serial.read();  //讀取指令
    if (instruction == 'S')  return false;//停止演奏
  }
  return true;//繼續演奏
}

