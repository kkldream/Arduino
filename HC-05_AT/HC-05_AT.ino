#include <SoftwareSerial.h>
SoftwareSerial BT(7, 8);  //TX,RX
char val;

void setup() {
  Serial.begin(9600);
  Serial.println("BT is ready!");
  BT.begin(38400);
}

void loop() {
  if (Serial.available()) {
    val = Serial.read();
    BT.print(val);
  }
  if (BT.available()) {
    val = BT.read();
    Serial.print(val);
  }
}
