#include <UsbKeyboard.h>
const byte KeyPin[6] = {A5, A4, A3, A2, A1, A0}, RGBPin[] = {11, 10}, RGB_Max = 50;
byte Key[6], PastKey[4], RGB, RGB_var1 = 1, RGB_mode2;
boolean PressKey, Mode, Mode_var1, RGB_var2, RGB_mode1, RGB_var3;
int Time, RGB_time;
void setup() {
  for (byte i = 0; i < 6; i++) pinMode(KeyPin[i], INPUT);
  for (byte i = 0; i < 2; i++) pinMode(RGBPin[i], OUTPUT);
  for (int i = 0; i < 1000; i++) {
    UsbKeyboard.update();
    delay(1);
  }
}
void RGB_mode(byte Mode) {
  if (Mode == 0) {
    if (RGB == RGB_Max)RGB_var1 = -1;
    else if (RGB < 1) RGB_var1 = 1;
  }
  else if (Mode == 1) {
    RGB_var1 = 0;
    RGB = RGB_Max;
  }
  else if (Mode == 2) {
    RGB_var1 = 0;
    RGB = 5;
  }
  else if (Mode == 3) {
    RGB_var1 = 0;
    RGB = 0;
  }
  if (RGB_time == 1000) {
    analogWrite(RGBPin[!RGB_mode1], 0);
    analogWrite(RGBPin[RGB_mode1], RGB += RGB_var1);
    RGB_time = 0;
  }
  else RGB_time++;
}
void loop() {
  RGB_mode(RGB_mode2);
  Time++;
  for (byte i = 0; i < 5; i++) if (digitalRead(KeyPin[i])) PressKey = 1;
  if (Time > 1000) {
    if (PressKey == 0) UsbKeyboard.sendKey(0, 0, 0, 0);
    Time = 0;
    PressKey = 0;
  }
  RGB_mode(RGB_mode2);
  if (digitalRead(KeyPin[5])) {
    while (digitalRead(KeyPin[5])) {
      if (digitalRead(KeyPin[0])) {
        if (Mode) {
          UsbKeyboard.sendKeyStroke(KEY_K);
          UsbKeyboard.sendKeyStroke(KEY_E);
          UsbKeyboard.sendKeyStroke(KEY_Y);
          UsbKeyboard.sendKeyStroke(46);
          UsbKeyboard.sendKeyStroke(33);
          UsbKeyboard.sendKeyStroke(KEY_SPACE);
        }
        else UsbKeyboard.sendKey(0, KEY_F3, 0, 0);
        Mode_var1 = 1;
        while (digitalRead(KeyPin[0]) && digitalRead(KeyPin[5])) RGB_mode(RGB_mode2);
        UsbKeyboard.sendKey(0, 0, 0, 0);
      }
      if (digitalRead(KeyPin[1])) {
        if (Mode) UsbKeyboard.sendKey(0, KEY_F2, 0, 0);
        else UsbKeyboard.sendKey(0, KEY_F4, 0, 0);
        Mode_var1 = 1;
        while (digitalRead(KeyPin[1]) && digitalRead(KeyPin[5])) RGB_mode(RGB_mode2);
        UsbKeyboard.sendKey(0, 0, 0, 0);
      }
      if (digitalRead(KeyPin[2])) {
        UsbKeyboard.sendKey(0, KEY_F8, 0, 0);
        Mode_var1 = 1;
        while (digitalRead(KeyPin[2]) && digitalRead(KeyPin[5])) RGB_mode(RGB_mode2);
        UsbKeyboard.sendKey(0, 0, 0, 0);
      }
      if (digitalRead(KeyPin[3])) {
        UsbKeyboard.sendKey(0, KEY_F9, 0, 0);
        Mode_var1 = 1;
        while (digitalRead(KeyPin[3]) && digitalRead(KeyPin[5])) RGB_mode(RGB_mode2);
        UsbKeyboard.sendKey(0, 0, 0, 0);
      }
      if (digitalRead(KeyPin[4])) {
        while (digitalRead(KeyPin[5])) {
          if (!digitalRead(KeyPin[4])) RGB_var3 = 0;
          else if (RGB_var3 == 0) {
            RGB_var3 = 1;
            RGB_mode2++;
          }
          if (RGB_mode2 == 4) RGB_mode2 = 0;
          RGB_mode(RGB_mode2);
          Time++;
          if (Time > 1000) {
            UsbKeyboard.sendKey(0, 0, 0, 0);
            Time = 0;
          }
        }
        Mode_var1 = 1;
      }
      RGB_mode(RGB_mode2);
    }
    if (Mode_var1 == 0) Mode = Mode == 0 ? 1 : 0;
    Mode_var1 = 0;
  }
  RGB_mode(RGB_mode2);
  if (Mode == 0) {
    RGB_mode1 = 0;
    if (digitalRead(KeyPin[4])) {
      if (!digitalRead(KeyPin[1])) UsbKeyboard.sendKey(0, 41, 0, 0);
      while (digitalRead(KeyPin[4])) {
        if (digitalRead(KeyPin[1])) {
          UsbKeyboard.sendKey(0, 53, 0, 0);
          while (digitalRead(KeyPin[1]) && digitalRead(KeyPin[4])) RGB_mode(RGB_mode2);
        }
        RGB_mode(RGB_mode2);
      }
      UsbKeyboard.sendKey(0, 0, 0, 0);
    }
    else {
      Key[0] = !digitalRead(KeyPin[0]) ? 0 : 1;
      Key[1] = !digitalRead(KeyPin[1]) ? 0 : KEY_Z;
      Key[2] = !digitalRead(KeyPin[2]) ? 0 : KEY_C;
      Key[3] = !digitalRead(KeyPin[3]) ? 0 : KEY_V;
      if (PastKey[0] != Key[0] || PastKey[1] != Key[1] || PastKey[2] != Key[2] || PastKey[3] != Key[3]) {
        UsbKeyboard.sendKey(Key[0], Key[1], Key[2], Key[3]);
        for (byte i = 0; i < 4; i++) PastKey[i] = Key[i];
      }
    }
  }
  else if (Mode == 1) {
    RGB_mode1 = 1;
    if (digitalRead(KeyPin[4])) {
      UsbKeyboard.sendKey(0, 40, 0, 0);
      while (digitalRead(KeyPin[4])) RGB_mode(RGB_mode2);
      UsbKeyboard.sendKey(0, 0, 0, 0);
    }
    if (digitalRead(KeyPin[0])) {
      UsbKeyboard.sendKey(0, 82, 0, 0);
      while (digitalRead(KeyPin[0])) RGB_mode(RGB_mode2);
      UsbKeyboard.sendKey(0, 0, 0, 0);
    }
    if (digitalRead(KeyPin[1])) {
      UsbKeyboard.sendKey(0, 81, 0, 0);
      while (digitalRead(KeyPin[1])) RGB_mode(RGB_mode2);
      UsbKeyboard.sendKey(0, 0, 0, 0);
    }
    if (digitalRead(KeyPin[2])) {
      UsbKeyboard.sendKey(0, 80, 0, 0);
      while (digitalRead(KeyPin[2])) RGB_mode(RGB_mode2);
      UsbKeyboard.sendKey(0, 0, 0, 0);
    }
    if (digitalRead(KeyPin[3])) {
      UsbKeyboard.sendKey(0, 79, 0, 0);
      while (digitalRead(KeyPin[3])) RGB_mode(RGB_mode2);
      UsbKeyboard.sendKey(0, 0, 0, 0);
    }
  }
}
