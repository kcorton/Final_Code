/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a global variable with the servo position at the highest flame sensing
// this is used once the flame has been seen

void scan(void) {

  int currFlameVal;

  // if the scan is complete
  if(fireServoPos > 180) {

    fireServoPos = 0; // reset the position
    if(lastFlameVal < 800) {
      fireStillExists = true;
    }
    else {
      fireStillExists = false;
    }
    lastFlameVal = 2000; // reset the last flame val
    flameServo.write(0); // write the servo back to reset position
    scanComplete = true; //Indicate the scan is complete
    if (firePosition > 90){
      candleTurn = (firePosition - 90);
    }
    else{
      candleTurn = -(90 -firePosition);
    }
    flameAngle += candleTurn;
  }

  // if the scan is not complete
  else {

    flameServo.write(fireServoPos);
    currFlameVal = analogRead(firePin);

    // if the current flame value is less than the previous lowest flame value set it to the new flame value
    if(currFlameVal < lastFlameVal) {

      lastFlameVal = currFlameVal;
      firePosition =  fireServoPos;

    }

    fireServoPos += 5;

  }

  lastFireTimeCount = countTime; // reset the stored time variable

}

/*********************************************************************************************/
// Turn Toward Flame function
// turns robot to the angle specified based on the variable changed in the scan function

void turnTowardFlame(void){

  // if we need to turn
  if(firePosition != 90){
    turn(candleTurn);
  }
  else {
    // do not need to turn, turn is complete
    turnComplete = true;
  }
}

/*********************************************************************************************/
// Drive to Candle Function

void driveToCandle(void) {
  switch (driveToCandleState) {
  case scanning:  // scan for the fire
    scan();

    if(scanComplete) {
      driveToCandleState = turningToCandle;
      scanComplete = false; 
    }
    break;

  case turningToCandle: 
    turnTowardFlame();   // turn towards where the fire was most intense 

    if(turnComplete){
      turnComplete = false;
      driveToCandleState = drivingForTime;
      initTimeCandleDrive = countTime;
    }

    break;


  case drivingForTime:    // drive for two seconds towards the flame
    driveStraightForwardEnc();
    if((countTime - initTimeCandleDrive) >= 20){
      stopAllDrive();
      driveToCandleState = updatingLocation;

    }

    break;

  case updatingLocation:  // update the location of the robot
    updateAngleDriveLocation();
    driveToCandleState = scanning;
    break;
  default:
    Serial.println(" HIT DRIVE TO CANDLE DEFAULT");
    lcd.println("ERROR 05");
    delay(5000);
    break;
  }

}


/*********************************************************************************************/
// Activate Fan Switches Function 
//drives the fan up until it reaches the limit switch, waits a set amount of time, then sends the fan down.

void activateFanSwitches(void){
  
  digitalWrite(fanPin, HIGH);  // turn the fan on
  switch(armStateSwitches){
  case drivingUp:
    armMotor.write(armRaiseSpeed); //raise the fan
    if(digitalRead(upperSwitchPin) == LOW){
      armMotor.write(armHoldSpeed); //hold the fan high
      initUpTime = millis();  
      armStateSwitches = waitingAtTop;
    }
    break;
    
  case waitingAtTop:
    // if the arm is not still contacting the limit switch
    if(digitalRead(upperSwitchPin) != LOW){
      armMotorAdjust++;  
    }
    armMotor.write(armHoldSpeed + armMotorAdjust);
    
    // if enough time has passed 
    if((millis() - initUpTime) > armUpTime){
      armStateSwitches = drivingDown;
    }
    break;
    
  case drivingDown:
    armMotor.write(armLowerSpeed);
    // if the lower switch is hit 
    if(digitalRead(lowerSwitchPin) == LOW){
      if(initDownTime == 0) {    
        initDownTime = millis();
      }
      else if((millis() - initDownTime) > armUpTime) {
        fanSweepComplete = true;  
        armMotorAdjust = 0;
        initUpTime = 0;
        initDownTime = 0;
        armStateSwitches = drivingUp;
        digitalWrite(fanPin, LOW);
      }
    }
    break;
    
  default: 
    Serial.println("Bran wants defaults");
    lcd.print("Error 13");
    delay(5000);
    break;
  }
}
/*********************************************************************************************/
// Report Flame Function 
// reports the location of the flame 

void reportFlame(void) {
  lcd.setCursor(0, 0);
  lcd.print("Flame Found at:");
  lcd.setCursor(0, 1);
  lcd.print("(");
  lcd.setCursor(1, 1);
  lcd.print((int)xCoord);
  lcd.setCursor(4, 1);
  lcd.print(", ");
  lcd.setCursor(7, 1);
  lcd.print((int)yCoord);
  lcd.setCursor(10, 1);
  lcd.print(")");
}








