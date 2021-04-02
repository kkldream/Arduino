#include <IRremote.h>
IRsend irsend;
byte Mode,ledPin=10;

void setup() {
  pinMode(10,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    digitalWrite(ledPin, 1);
    Mode = Serial.read();
    if (Mode == '1')irsend.sendNEC(0x80FFC13E, 32);  //開/關
    if (Mode == '2')irsend.sendNEC(0x80FFE11E, 32);  //風速
    if (Mode == '3')irsend.sendNEC(0x80FFF10E, 32);  //自然/睡眠風
    if (Mode == '4')irsend.sendNEC(0x80FFD12E, 32);  //定時
    digitalWrite(ledPin, 0);
  }
}
