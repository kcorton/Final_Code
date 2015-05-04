/*********************************************************************************************/
// Left Motor Encoder ISR
// every time the left encoder is high increment the leftCounter 

void leftTick(){
  if (digitalRead(leftEncoderB) == HIGH){
    leftCounter++;
  }
  else{
    leftCounter--;
  }
}

/*********************************************************************************************/
// Right Motor Encoder ISR
// every time the right encoder is high increment the leftCounter 

void rightTick(){
  if (digitalRead(rightEncoderB) == HIGH){
    rightCounter--;
  }
  else{
    rightCounter++;
  }
}

/*********************************************************************************************/
// Timer ISR
// Increments the overall CountTime variable every tenth of a second
// also if accelTime is less than 1 increments it to accelerate the robot to it's top driving speed

void timerISR(){
  countTime++;
  if (accelTime < 1){
    accelTime += .05;                             
  }
}

/*********************************************************************************************/
// Front Sonar ISR
// when the ping gets back to the front sensor this is called 


void frontSonarISR(){
  if(digitalRead(frontEchoPin) == HIGH){
    frontPingTime = micros();
  }
  else{
    tempEchoFront = micros() - frontPingTime;
    if((tempEchoFront < 30000) && (tempEchoFront >=0)){
      frontEchoTime = tempEchoFront;
      switch(mainState){
      case findingFire:
        {
          pingNext = sideSonar;
          break;
        }
      case extinguishingFire:
        {
          pingNext = frontSonar;
          break;
        }
      case returningHome:
        {
          pingNext = backSonar;
          break;
        }
      case madeItHome:
        {
          pingNext = frontSonar;
          break;
        }
      default:
        Serial.println("HIT FRONT SONAR ISR DEFAULT");
        lcd.println("ERROR 10");
        delay(5000);
        break;
      }
    }
    else{
      pingNext = frontSonar; 
    }
    waiting = false; 
  }

}

