#include <Arduino.h>
/*13 12 11 10 9 8  7  6  5  4  3  2       A4
  e  d  D4 a  f D3 D2 b  c  g  D1 按鈕  可變電阻
                                  按下為0                          
*/

void seven_seg_scan_display_one(int num, int data);
void seven_seg_scan_display_all(int num);

const int flash_times = 3;
const int flash_delay = 250;
const int seven_seg_scan_pin[] = {11, 8, 7, 3};
const int seven_seg_data_pin[] = {10, 6, 5, 12, 13, 9, 4};
const int seven_seg_encoder[10][7] = {
  {0, 0, 0, 0, 0, 0, 1}, // 0
  {1, 0, 0, 1, 1, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0}, // 2
  {0, 0, 0, 0, 1, 1, 0}, // 3
  {1, 0, 0, 1, 1, 0, 0}, // 4
  {0, 1, 0, 0, 1, 0, 0}, // 5
  {0, 1, 0, 0, 0, 0, 0}, // 6
  {0, 0, 0, 1, 1, 1, 1}, // 7
  {0, 0, 0, 0, 0, 0, 0}, // 8
  {0, 0, 0, 0, 1, 0, 0}  // 9
};

void setup() {
  for(int i = 0; i < 4; i++)
    pinMode(seven_seg_scan_pin[i], OUTPUT);
  for(int i = 0; i < 7; i++)
    pinMode(seven_seg_data_pin[i], OUTPUT);
  for (int i = 0; i < flash_times; i++) {
    long time_temp = millis();
    while (millis() - time_temp < flash_delay)
      seven_seg_scan_display_all(9900);
    seven_seg_scan_display_close();
    delay(flash_delay)
  }
}

bool button_temp = false;
int press_times = 0;
int counter_num = 99;
long time_temp = millis();
void loop() {
  int counter_speen = map(analogRead(A4), 0, 1023, 50, 5000);
  if (millis() - time_temp > counter_speen && counter_num > 0) {
    time_temp = millis();
    counter_num--;
  }
  seven_seg_scan_display_one(2, counter_num / 100 % 10);
  seven_seg_scan_display_one(3, counter_num / 1000 % 10);
  
  if (digitalRead(2)) button_temp = false;
  else if (!button_temp) {
    button_temp = true;
    if (counter_num > 0)
      press_times++;
  }
  seven_seg_scan_display_one(0, press_times / 10);
  seven_seg_scan_display_one(1, press_times / 10 % 10);
}

void seven_seg_scan_display_one(int num, int data) {
  for(int i = 0; i < 4; i++)
    if (i != num)
      digitalWrite(seven_seg_scan_pin[i], LOW);
  for(int i = 0; i < 7; i++)
    digitalWrite(seven_seg_data_pin[i], seven_seg_encoder[data][i]);
  digitalWrite(seven_seg_scan_pin[num], HIGH);
  delay(5);
}

void seven_seg_scan_display_all(int num) {
  seven_seg_scan_display_one(0, num / 10);
  seven_seg_scan_display_one(1, num / 10 % 10);
  seven_seg_scan_display_one(2, num / 100 % 10);
  seven_seg_scan_display_one(3, num / 1000 % 10);
}

void seven_seg_scan_display_close() {
  for(int i = 0; i < 4; i++)
    digitalWrite(seven_seg_scan_pin[i], LOW);
}
