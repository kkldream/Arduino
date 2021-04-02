#include <Wire.h>
void setup() {
  Wire.begin(0x20);
  Wire.onReceive(receiveEvent);
  Serial.begin(250000);
  Serial.println("Type something to send:");
}
void loop() {
}
void serialEvent() {
  Wire.beginTransmission(0x04);
  Wire.write(Serial.read());
  Wire.endTransmission();
}
void receiveEvent() {
  while (Wire.available()) {
    int var = Wire.read();
    Serial.println(var);
  }
}
