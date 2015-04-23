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
      pingNext = sideSonar;
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
      pingNext = frontSonar;
    }
    else{
      pingNext = sideSonar; 
    }
    waiting = false;
  }
}
//========================================================
void backSonarISR(){
  if(digitalRead(backEchoPin) == HIGH){
    backPingTime = micros();
  }
  else{
    tempEchoBack = micros() - backPingTime;
    if(tempEchoBack < 30000){
      backEchoTime = tempEchoBack;
      pingNext = frontSonar;
    }
    else{
      pingNext = frontSonar; 
    }
    waiting = false;
  }
}
