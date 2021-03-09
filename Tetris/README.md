用左邊的
![](https://img.ruten.com.tw/s1/3/bb/d1/21607801650129_84.jpg)

![](https://electronoobs.com/images/Arduino/tut_14/schematic_1_logo.png)

![](https://components101.com/sites/default/files/component_pin/MAX7219-IC-Pinout.jpg)

https://atceiling.blogspot.com/2017/03/arduinomax7219-8x8-led-matrix.html

Demo Code：
```c++
#include <LedControl.h>
LedControl lc = LedControl(12, 11, 10, 1); 　 // (Data, Clock , Cs, Num)
void setup() {
  lc.shutdown(0, false); // 關閉省電模式
  lc.setIntensity(0, 8); // 設定亮度為 8 (介於0~15之間)
  lc.clearDisplay(0);    // 清除螢幕
}
void loop() {
  for (int a = 0; a < 64; a++) {
    lc.setLed(0, a / 8, a % 8, 1);
    delay(100);
  }
  lc.clearDisplay(0);
}
```