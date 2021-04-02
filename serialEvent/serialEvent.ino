int incomingByte;
String inputString;
boolean newLineReceived;
void setup() {
  Serial.begin(250000);
}
void loop() {
  if (newLineReceived) {
    Serial.print(inputString);
    inputString = "";
    newLineReceived = 0;
  }
}
void serialEvent() {
  while (Serial.available()) {
    incomingByte = Serial.read();
    inputString += (char) incomingByte;
    if (incomingByte == '\n') newLineReceived = 1;
  }
}
