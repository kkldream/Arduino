const int button_pin = 2;
void setup() {
  Serial.begin(250000);
}
void loop() {
  if (digitalRead(button_pin)) Serial.println(ButtonPressTimes());
}
int ButtonPressTimes() {
  byte PressTimes;
  const int DelayTime = 250;  //連按間隔限制時間
  while (digitalRead(button_pin)) {
    PressTimes++;
    while (digitalRead(button_pin));
    unsigned long Press_millis = millis();
    while (millis() - Press_millis < DelayTime && !digitalRead(button_pin));
  }
  return PressTimes;
}
