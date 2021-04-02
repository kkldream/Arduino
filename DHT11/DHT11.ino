#include <dht.h>  //下載點:https://goo.gl/G0Waqe
dht DHT11;
const byte dht_pin = 6;
void setup() {
  Serial.begin(250000);
}
void loop() {
  DHT11.read11(dht_pin);
  Serial.print("Temperature=");
  Serial.print(DHT11.temperature);
  Serial.print(".C   Humidity=");
  Serial.print(DHT11.humidity);
  Serial.println("%");
  delay(1000);
}
