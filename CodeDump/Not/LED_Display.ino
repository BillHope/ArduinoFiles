#include <LiquidCrystal.h>

LiquidCrystal lcd(12,11,5,4,3,2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.print("Woo Hoo");
  Serial.println("Woo Hoo");
  delay(10000);
  lcd.noDisplay();
  lcd.display();
  delay(3000);
}
