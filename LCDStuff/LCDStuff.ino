#include <LiquidCrystal.h>

/* Set LCD Enable ?high? - might be active low


Pin settings are as follows:
 * LCD RS pin to digital pin 13
 * LCD Enable pin to digital pin 12
 * LCD D4 to digital pin 17
 * LCD D5 to digital pin 16
 * LCD D6 to digital pin 15
 * LCD D7 to digital pin 14

*/
LiquidCrystal lcd(13, 12, 17, 16, 15, 14);

float xCoord = 0;
float yCoord = 0;

void setup() {
lcd.begin(16, 2);
}

void loop() {
  printPosition();
  delay(100);
}


void printPosition() {
  
  lcd.setCursor(0, 0);
  lcd.print("(");
  lcd.setCursor(1, 0);
  lcd.print((int)xCoord);
  lcd.setCursor(4, 0);
  lcd.print(", ");
  lcd.setCursor(7, 0);
  lcd.print((int)yCoord);
  lcd.setCursor(10, 0);
  lcd.print(")");
  
}
