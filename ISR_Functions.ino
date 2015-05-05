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
// Triggers when the line changes. Initially, the line is low. When the line goes high,
//  the ping has been sent, so the initial time is set using micros()
// When the line goes back to low, the echo has been recieved, so the travel time is
//  calculated, again using micros()

void frontSonarISR(){
  if(digitalRead(frontEchoPin) == HIGH){
    //the ping has just been sent, so set the initial ping time
    frontPingTime = micros();
  }
  else{
    //the echo has just been recieved, so calculate the travel time
    tempEchoFront = micros() - frontPingTime;
    
    //if the time calculated makes sense, save that time
    // (gets rid of random max values and random negative values)
    if((tempEchoFront < 30000) && (tempEchoFront >=0)){
      frontEchoTime = tempEchoFront;
      
      //calculate which sonar to ping next
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
    
    //an undesirable time was calculated, so re-ping the front sensor
    else{
      pingNext = frontSonar; 
    }
    // tell the ping function the echo has been recieved
    waiting = false; 
  }

}

