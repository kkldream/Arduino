void setup() {
  Serial.begin(250000);
}
void loop() {
  if (Serial.available()) Serial.println(Serial_receive());
}
byte Serial_receive() {
  byte var;
  byte chr, digit, data[3];
  do {
    chr = Serial.read();
    if (chr >= '0' && chr <= '9' && digit < 3) {
      data[digit] = chr - 48;
      digit++;
    }
    else data[digit] = 10;
  } while (chr != '\n');
  if (data[0] == 10) var = 0;
  else if (data[1] == 10) var = data[0];
  else if (data[2] == 10) var = data[0] * 10 + data[1];
  else var = data[0] * 100 + data[1] * 10 + data[2];
  return var;
}
