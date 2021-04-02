int Time, led_status, switch_pin = 2, led_pin = 13;
void setup()
{
  pinMode(switch_pin, INPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
}
void loop()
{
  if (digitalRead(switch_pin) == HIGH && millis() - Time > 200)
  {
    Time = millis();
    led_status = led_status == HIGH ? LOW : HIGH;
    digitalWrite(led_pin, led_status);
  }
}
