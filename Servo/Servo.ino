#include <Servo.h>
#include <Wire.h>
Servo myservo;
byte angle;

void setup() {
  Serial.begin(250000);
  Serial.println("Start");
  myservo.attach(4);
  Turn(90);
}
void loop() {
  if (Serial.available()) {
    byte angle = Serial.parseInt();
//    byte angle = Serial_receive();
    Turn(angle);
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
void Turn(byte a) {
  Serial.print("Angle = ");
  Serial.println(a);
  myservo.write(a);
}
