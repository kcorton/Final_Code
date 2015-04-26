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
}

//========================================================
void frontSonarISR(){
  if(digitalRead(frontEchoPin) == HIGH){
    frontPingTime = micros();
  }
  else{
    tempEchoFront = micros() - frontPingTime;
    if(tempEchoFront < 30000){
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
          pingNext = sideSonar;
          break;
        }
      case madeItHome:
        {
          pingNext = frontSonar;
          break;
        }
      }
    }
    else{
      pingNext = frontSonar; 
    }
    waiting = false; 
  }

}
//========================================================
void sideSonarISR(){
  if(digitalRead(sideEchoPin) == HIGH){
    sidePingTime = micros();
  }
  else{
    tempEchoSide = micros() - sidePingTime;
    if(tempEchoSide < 30000){
      sideEchoTime = tempEchoSide;
      switch(mainState){
      case findingFire:
        {
          pingNext = frontSonar;
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
      }
    }
    else{
      pingNext = sideSonar; 
    }
    waiting = false;
  }
}



