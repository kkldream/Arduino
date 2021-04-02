#include <VirtualWire.h>
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
void setup() {
  Serial.begin(250000);
  vw_set_ptt_inverted(true);
  vw_setup(9600);
  vw_set_tx_pin(8);
  pinMode(13, INPUT);
}
void loop() {
  if (digitalRead(13) == HIGH) vw_send("A", 1);
  else vw_send("B", 1);
}
