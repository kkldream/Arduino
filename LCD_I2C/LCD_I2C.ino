#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 2);
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("Hello, world!");
}
void loop() {
  //lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
  delay(1000);
}

