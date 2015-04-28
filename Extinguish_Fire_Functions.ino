/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a global variable with the servo position at the highest flame sensing
// this is used once the flame has been seen

void scan(void) {

  int currFlameVal;

  //  if(countTime - lastFireTimeCount > 1) {

  if(fireServoPos > 180) {

    fireServoPos = 0; // reset the position
    lastFlameVal = 2000; // reset the last flame val
    flameServo.write(0); // write the servo back to reset position
    scanComplete = true; //Indicate the scan is complete

      //Serial.println("DONEEEEE");
    //Serial.println(firePosition);

  }

  else {

    flameServo.write(fireServoPos);
    currFlameVal = analogRead(firePin);

    //Serial.print("Position ");
    //Serial.print(fireServoPos);
    //Serial.print("Flame Value ");
    //Serial.println(currFlameVal);

    if(currFlameVal < lastFlameVal) {

      lastFlameVal = currFlameVal;
      firePosition =  fireServoPos;

    }

    fireServoPos += 5;

  }

  lastFireTimeCount = countTime; // reset the stored time variable

    //  }

}

/*********************************************************************************************/
// Turn Toward Flame function
// turns robot to the angle specified based on the variable changed in the scan function

void turnTowardFlame(void){
  if (firePosition > 90){
    candleTurn = (firePosition - 90);
    turn(candleTurn);

  }
  else if (firePosition < 90){
    candleTurn = -(90 -firePosition);
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
      driveToCandleState = drivingForTime;
      initTimeCandleDrive = countTime;
    }

    break;


  case drivingForTime:
    driveStraightForwardEnc();
    if((countTime - initTimeCandleDrive) >= 20){
      stopAllDrive();
      driveToCandleState = updatingLocation;

    }

    break;

  case updatingLocation:
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
// Activate Fan Function 

// Sweeps Fan between it's two extreme postions and checks if the fire is still detected 

void activateFanPID(void) {
  digitalWrite(fanPin, HIGH);
  armPos = analogRead(armPotPin);
  switch (armState) {
  case armUp:
    armWrite = 410;
    if (armPos >= armWrite) {
      armState = armDown;
    }
    break;

  case armDown:
    armWrite = 310;
    if (armPos <= armWrite) {
      armState = armUp;
      fanSweepComplete = true;
    }
    break;
  }
  armMotor.write(pidOut);
  //Serial.println(armPos);

  //  // run fan between two extremes 
  //  digitalWrite(fanPin, HIGH);
  //  armTimePassed = countTime - armInitTime;
  //  switch(armState){
  //  case armToMid:
  //    armMotor.write(108);
  //    if(analogRead(armPotPin) > midPos){
  //      armState = armAtMid;
  //    }
  //    break;
  //  case armAtMid:
  //    initTime = countTime;
  //    armState = waitingAtMid;
  //    break;
  //  case waitingAtMid:
  //    armMotor.write(100);
  //    if((armTimePassed - initTime) > armWaitTime){
  //      armState = armToTop;
  //    }
  //    break;
  //  case armToTop:
  //    armMotor.write(108);
  //    if(analogRead(armPotPin) > highPos){
  //      armState = reachedTop;
  //    }
  //    break;
  //  case reachedTop:
  //    initTime = countTime;
  //    armState = waitingAtTop;
  //    break;
  //  case waitingAtTop: 
  //    armMotor.write(100);
  //    if((armTimePassed-initTime) > armWaitTime){
  //      armState = loweringArm;
  //    }
  //    break;
  //  case loweringArm:
  //    armMotor.write(93);
  //    if (analogRead(armPotPin) <=  lowPos){
  //      armMotor.write(90);
  //      digitalWrite(fanPin, LOW);
  //      fanSweepComplete = true;
  //    }
  //    break;
  //
  //  default:
  //    Serial.println("HIT ACTIVATE FAN DEFAULT");
  //    lcd.println("ERROR 06");
  //    delay(5000);
  //    break;
  //  }
}
/*********************************************************************************************/
void calcArmPID() {

  int integral;
  integral += armPos;
  int lastPos = armPos;
  armPos = analogRead(armPotPin);
  int pidRaw = (float)Kp * (armWrite - armPos) + (float)Kd * (lastPos - armPos) + (float)Ki * integral;
  if (pidRaw < 0) {
    pidOut = map(pidRaw, 0, 90, -2000, 0);
  }
  else {
    pidOut = map(pidRaw, 90, 180, 0, 2000);
  }
}


/*********************************************************************************************/
//drives the fan up until it reaches the limit switch, waits a set amount of time, then sends the fan down.
void activateFanSwitches(void){
  Serial.print(armStateSwitches);
  Serial.print("  ");
  Serial.print((int)digitalRead(lowerSwitchPin));
  Serial.print("  ");
  Serial.print((int)digitalRead(upperSwitchPin));
  
  digitalWrite(fanPin, HIGH);
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
    if(digitalRead(upperSwitchPin) != LOW){
      armMotorAdjust++;  
    }
    armMotor.write(armHoldSpeed + armMotorAdjust);
    if((millis() - initUpTime) > armUpTime){
      armStateSwitches = drivingDown;
    }
    break;
  case drivingDown:
    armMotor.write(armLowerSpeed);
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
  lcd.print("(");
  lcd.setCursor(1, 0);
  lcd.print((int)xCoord);
  lcd.setCursor(4, 0);
  lcd.print(", ");
  lcd.setCursor(7, 0);
  lcd.print((int)yCoord);
  lcd.setCursor(10, 0);
  lcd.print(")");
}


/*********************************************************************************************/
// Updating Angle Drive Location 
// this updates the x and y corrdinates based on the angle the robot was driving at towards the candle

void updateAngleDriveLocation(void){

  if (drivingDirection == xPos){
    if (candleTurn > 0){

      xCoord = xCoord + candleChangeDisCos();
      yCoord = yCoord + candleChangeDisSin();
    }

    else if(candleTurn < 0){
      xCoord = xCoord + candleChangeDisCos();
      yCoord = yCoord - candleChangeDisSin();
    }
  }
  else if(drivingDirection == yPos) {
    if (candleTurn > 0){

      xCoord = xCoord - candleChangeDisSin();
      yCoord = yCoord + candleChangeDisCos();
    }

    else if(candleTurn < 0){
      xCoord = xCoord + candleChangeDisSin();
      yCoord = yCoord + candleChangeDisCos();
    }
  }
  else if( drivingDirection == xNeg) {
    if (candleTurn > 0){

      xCoord = xCoord - candleChangeDisCos();
      yCoord = yCoord - candleChangeDisSin();
    }

    else if(candleTurn < 0){
      xCoord = xCoord - candleChangeDisCos();
      yCoord = yCoord + candleChangeDisSin();
    }
  }
  else if(drivingDirection == yNeg) {
    if (candleTurn > 0){

      xCoord = xCoord + candleChangeDisSin();
      yCoord = yCoord - candleChangeDisCos();
    }

    else if(candleTurn < 0){
      xCoord = xCoord - candleChangeDisSin();
      yCoord = yCoord - candleChangeDisCos();
    }
  }

  leftCounter = 0;
  rightCounter = 0;
  lastLeftTicks = 0; 
  lastRightTicks = 0;

}

/*********************************************************************************************/

float candleChangeDisCos(void){
  float leftDist = 0;
  float rightDist = 0;

  leftDist = inchesPerTick * leftCounter;
  rightDist = inchesPerTick * rightCounter;

  return (cos((leftDist + rightDist) / 2)) ; 
}
/*********************************************************************************************/

float candleChangeDisSin(void){
  float leftDist = 0;
  float rightDist = 0;

  leftDist = inchesPerTick * leftCounter;
  rightDist = inchesPerTick * rightCounter;

  return (sin((leftDist + rightDist) / 2)) ;  
}




