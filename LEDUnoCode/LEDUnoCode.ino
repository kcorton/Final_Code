/*Upload to the Uno to run the LED's

These LED's will first make a circular pattern around the top of the robot,
then they will all flash together 4 times. This cycle will then repeat itself

Written by Charlie Sinkler
*/

int state = 0;
int timesThroughSpin = 0;
int timesThroughFlash = 0;

void setup() {
  pinMode(11, INPUT_PULLUP);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
}

void loop() {

  switch (state) {
    case 0:
      if (digitalRead(11) == LOW) {
        state = 1;
      }
      spin();
      break;

    case 1:
      if (digitalRead(11) == HIGH) {
        state = 0;
      }
      flash();
      break;

      //    case 2:
      //      allOff();
      //      if (digitalRead(11) == LOW) {
      //        state = 0;
      //      }
  }
}

void spin() {
  for (int pin = 1; pin <= 10; pin++) {
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin, LOW);
  }
}

void flash() {
  for (int pin = 1; pin <= 10; pin++) {
    if(pin % 2) {
      digitalWrite(pin, HIGH);
    }
    else {
      digitalWrite(pin, LOW);
    }
  }
  delay(100);
  for (int pin = 1; pin <= 10; pin++) {
    if(pin % 2) {
      digitalWrite(pin, LOW);
    }
    else {
      digitalWrite(pin, HIGH);
    }
  }
  delay(100); 
}

void backAndForth() {
  for (int pin = 1; pin <= 10; pin++) {
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin, LOW);
  }
  for (int pin = 10; pin >= 1; pin--) {
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin, LOW);
  }
  
}
  

//void allOff() {
//  for (int pin = 1; pin <= 10; pin++) {
//    digitalWrite(pin, LOW);
//  }
//}

