#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);  //LCD接腳: RS, Enable, D4, D5, D6, D7
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Welcome");
  lcd.setCursor(0, 1);
}
void loop() {
  if (Serial.available()) lcd.print(char(Serial.read()));
}
