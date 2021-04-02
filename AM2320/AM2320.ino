#include <AM2321.h> //下載點:https://goo.gl/XWOp2i
AM2321 AM2320;
void setup() {
  Serial.begin(250000);
}
void loop() {
  AM2320.read();
  Serial.print("Temperature=");
  Serial.print(AM2320.temperature/10);
  Serial.print(".C   Humidity=");
  Serial.print(AM2320.humidity/10);
  Serial.println("%");
  delay(1000);
}
