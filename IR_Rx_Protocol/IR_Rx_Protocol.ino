#include <IRremote.h> //下載點:https://goo.gl/rb0mXC
IRrecv irrecv(2);
decode_results results;
void setup() {
  Serial.begin(250000);
  irrecv.enableIRIn();
}
void loop() {
  if (irrecv.decode(&results)) {
    { if (results.decode_type = NEC) Serial.print("NEC");
      else if (results.decode_type = SONY) Serial.print("SONY");
      else if (results.decode_type = RC5) Serial.print("RC5");
      else if (results.decode_type = RC6) Serial.print("RC6");
      else Serial.print("Unknown");
    }
    Serial.print(" , ");
    Serial.print(results.value, HEX);
    Serial.print(" , ");
    Serial.print(results.bits);
    Serial.println(" bits");
    irrecv.resume();
  }
}
