/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a global variable with the servo position at the highest flame sensing
// this is used once the flame has been seen
// turns a global variable scanComplete to 1 once it has finished

void scan(void) {
  
  // if scan is complete 
  scanComplete = true;

}

/*********************************************************************************************/
// Turn Toward Flame function
// turns robot to the angle specified based on the variable changed in the scan function

void turnTowardFlame(void){

}

/*********************************************************************************************/
// Drive to Candle Function

void driveToCandle(void) {


  scan(); 
  turnTowardFlame();
  driveStraightForwardEnc();

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
