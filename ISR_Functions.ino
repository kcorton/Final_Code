/*********************************************************************************************/
// Left Motor Encoder ISR

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

void timerISR(){
  countTime++;
  if (accelTime < 1){
    accelTime += .05;                             
  }
}

//========================================================
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

