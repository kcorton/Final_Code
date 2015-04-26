#include <LiquidCrystal.h>

/* Set LCD Enable ?high? - might be active low


Pin settings are as follows:
LCD D4 to digital pin 22
LCD D5 to digital pin 23
LCD D6 to digital pin 24
LCD D7 to digital pin 25

*/
LiquidCrystal lcd(PINSGOHERE);

void setup() {
lcd.begin(22, 23, 24, 25);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print(xCoord);
  lcd.setCursor(0, 1);
  lcd.print(yCoord);
}
