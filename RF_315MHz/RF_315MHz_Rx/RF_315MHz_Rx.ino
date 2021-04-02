#include <VirtualWire.h>
uint8_t buf[VW_MAX_MESSAGE_LEN];
uint8_t buflen = VW_MAX_MESSAGE_LEN;
byte vw_rx_char;
void setup() {
  Serial.begin(250000);
  vw_set_ptt_inverted(true);
  vw_setup(9600);
  vw_set_rx_pin(8);
  vw_rx_start();
  pinMode(13, OUTPUT);
}
int a;
void loop() {
  if (vw_get_message(buf, &buflen)) vw_rx_char = buf[0];
  Serial.println(vw_rx_char);
  if (vw_rx_char == 'A') digitalWrite(13, HIGH);
  else digitalWrite(13, LOW);
}
