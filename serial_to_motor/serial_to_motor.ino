const byte motor_pin[] = {9, 10};
int incomingByte;
String inputString;
boolean newLineReceived;
void setup() {
  Serial.begin(250000);
  for (int a = 0; a < 2; a++) pinMode(motor_pin[a], OUTPUT);
  digitalWrite(motor_pin[0], 0);
}
void loop() {
  if (newLineReceived) {
    int var = inputString.toInt();
    inputString = "";
    newLineReceived = 0;
    Serial.println(var);
    analogWrite(motor_pin[1], var);
  }
}
void serialEvent() {
  while (Serial.available()) {
    incomingByte = Serial.read();
    inputString += (char) incomingByte;
    if (incomingByte == '\n') newLineReceived = 1;
  }
}
