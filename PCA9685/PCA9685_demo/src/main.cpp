#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
 
// 呼叫伺服驅動程式函數，預設I2C位址為 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define MIN_PULSE_WIDTH       544
#define MAX_PULSE_WIDTH       2400
#define FREQUENCY             50

// 定義伺服機使用的輸出Port
int motorA = 0;

void setup() {
  Serial.begin(250000);
  Serial.println("16 channel PWM test!");
  pwm.begin();
  pwm.setPWMFreq(FREQUENCY);  // This is the maximum PWM frequency
}
 
void loop() {
  pwm.setPWM(motorA, 0, pulse_width);
}