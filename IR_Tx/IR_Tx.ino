#include <IRremote.h>
const byte button_pin = 4;
IRsend irTx;
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
  if (digitalRead(button_pin)) {
    digitalWrite(LED_BUILTIN, HIGH);
    irTx.sendNEC(0x80FFC13E, 32);
    delay(200);
  }
  else digitalWrite(LED_BUILTIN, LOW);
}

