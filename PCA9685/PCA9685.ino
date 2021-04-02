#include <Wire.h>
const byte PCA9685_address = 0x40;
void setup() {
  Serial.begin(250000);
  Wire.begin();
  PCA9685_begin(50);
  Serial.println("Power on");
  for (byte a = 0; a < 16; a++)angleWrite(a, 90);
}
void loop() {
  if (Serial.available()) {
    byte angle = Serial_receive();
    angleWrite(15, angle);
    Serial.println(angle);
  }
}
byte Serial_receive() {
  byte var;
  byte chr, digit, data[3];
  do {
    chr = Serial.read();
    if (chr >= '0' && chr <= '9' && digit < 3) {
      data[digit] = chr - 48;
      digit++;
    }
    else data[digit] = 10;
  } while (chr != '\n');
  if (data[0] == 10) var = 0;
  else if (data[1] == 10) var = data[0];
  else if (data[2] == 10) var = data[0] * 10 + data[1];
  else var = data[0] * 100 + data[1] * 10 + data[2];
  return var;
}
void angleWrite(byte num, byte angle) {
  int pwm = map(angle, 0, 180, 480, 100);
  Wire.beginTransmission(PCA9685_address);
  Wire.write(6 + 4 * num);
  Wire.write(0);
  Wire.write(0);
  Wire.write(pwm);
  Wire.write(pwm >> 8);
  Wire.endTransmission();
}
void PCA9685_begin(int frequency) {
  Wire.beginTransmission(PCA9685_address);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission();
  Wire.beginTransmission(PCA9685_address);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(PCA9685_address, 1);
  byte oldmode = Wire.read();
  byte newmode = (oldmode & 0x7F) | 0x10;
  Wire.beginTransmission(PCA9685_address);
  Wire.write(0);
  Wire.write(newmode);
  Wire.endTransmission();
  Wire.beginTransmission(PCA9685_address);
  Wire.write(0xFE);
  Wire.write(byte(25000000 / 4096 / (frequency * 0.9) - 1 + 0.5));
  Wire.endTransmission();
  Wire.beginTransmission(PCA9685_address);
  Wire.write(0);
  Wire.write(oldmode);
  Wire.endTransmission();
  delay(5);
  Wire.beginTransmission(PCA9685_address);
  Wire.write(0);
  Wire.write(oldmode | 0xa1);
  Wire.endTransmission();
}
