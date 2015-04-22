/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a global variable with the servo position at the highest flame sensing
// this is used once the flame has been seen

void scan(void) {
  
  int currFlameVal;
  
  if(countTime - lastFireTimeCount > 28) {
  
    if(fireServoPos > 180) {
      
      fireServoPos = 0; // reset the position
      lastFlameVal = 2000; // reset the last flame val
      flameServo.write(0); // write the servo back to reset position
      scanComplete = true; //Indicate the scan is complete
      
      Serial.println("DONEEEEE");
      Serial.println(firePosition);
      
    }
    
    else {
      
      flameServo.write(fireServoPos);
      currFlameVal = analogRead(firePin);
      
      Serial.print("Position ");
      Serial.print(fireServoPos);
      Serial.print("Flame Value ");
      Serial.println(currFlameVal);
      
      if(currFlameVal < lastFlameVal) {
        
        lastFlameVal = currFlameVal;
        firePosition =  fireServoPos;
      
      }
      
      fireServoPos += 10;
      
    }
      
      lastFireTimeCount = countTime; // reset the stored time variable
    
  }
    
}

/*********************************************************************************************/
// Turn Toward Flame function
// turns robot to the angle specified based on the variable changed in the scan function

void turnTowardFlame(void){

}

/*********************************************************************************************/
// Drive to Candle Function

void driveToCandle(void) {
  switch (driveToCandleState) {
    case scanning:
    scan();
    
    if(scanComplete) {
      driveToCandleState = turningToCandle;
      scanComplete = false; 
    }
    break;
    
    case turningToCandle: 
        turnTowardFlame();

    if(turnComplete){
      turnComplete = false;
      extState = drivingForTime;
    }

    break;
    
    
    case drivingForTime:
    driveStraightForwardEnc();
    // if it's been a certain amount of time 
    driveToCandleState = updatingLocation;
    
    break;
    
    case updatingLocation:
      updateAngleDriveLocation();
      driveToCandleState = scanning;
      break;
  }  

}

/*********************************************************************************************/
// Activate Fan Function 

// Sweeps Fan between it's two extreme postions and checks if the fire is still detected 

void activateFan(void) { 

  // run fan between two extremes 
  digitalWrite(fanMotorPin, HIGH);
  armTimePassed = countTime - armInitTime;
  switch(armState){
  case raisingArm:
    armMotor.write(108);
    if(analogRead(armPotPin) > highPos){
      armState = reachedTop;
    }
    break;
  case reachedTop:
    initTime = countTime;
    armState = waitingAtTop;
    break;
  case waitingAtTop: 
    armMotor.write(100);
    if((armTimePassed-initTime) > armWaitTime){
      armState = loweringArm;
    }
    break;
  case loweringArm:
    armMotor.write(93);
    if (analogRead(armPotPin) <=  lowPos){
      armMotor.write(90);
      digitalWrite(fanMotorPin, LOW);
      fanSweepComplete = true;
    }
    break;
  }
}

/*********************************************************************************************/
// Report Flame Function 

// reports the location of the flame 

void reportFlame(void) {

}


/*********************************************************************************************/
// Updating Angle Drive Location 
// this updates the x and y corrdinates based on the angle the robot was driving at towards the candle

void updateAngleDriveLocation(void){
  
}

