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
//  if (firePosition > 90){
//    candleTurn = (firePosition - 90);
//    turn(candleTurn);
//
//  }
//  else if (firePosition < 90){
//    candleTurn = -(90 -firePosition);
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
  //Serial.print(armStateSwitches);
 // Serial.print("  ");
 // Serial.print((int)digitalRead(lowerSwitchPin));
 // Serial.print("  ");
  //Serial.print((int)digitalRead(upperSwitchPin));
  
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


/*********************************************************************************************/
// Updating Angle Drive Location 
// this updates the x and y corrdinates based on the angle the robot was driving at towards the candle

void updateAngleDriveLocation(void){
  averageDist();
  xCoord = xCoord + candleChangeDisX();
  yCoord = yCoord + candleChangeDisY();
  
  Serial.print(candleChangeDisX());
  Serial.print("  ");
  Serial.print(candleChangeDisY());
  Serial.print("  ");
  Serial.print(drivingDirection);
  Serial.print("  ");

  leftCounter = 0;
  rightCounter = 0;
  lastLeftTicks = 0; 
  lastRightTicks = 0;

}

/*********************************************************************************************/

void averageDist(void){
  float leftDist = 0;
  float rightDist = 0;

  leftDist = inchesPerTick * leftCounter;
  rightDist = inchesPerTick * rightCounter;
  
  averageDistance = ((leftDist + rightDist) / 2);
}

/*********************************************************************************************/

float candleChangeDisX(void){
  
  float cosAngle;
  cosAngle = cos(convertToRad(flameAngle));
//  Serial.print(cosAngle);
//  Serial.print("  ");
//  Serial.print(averageDistance);
//  Serial.print("  ");
  return (float)(averageDistance) * cosAngle;
  
}
/*********************************************************************************************/

float candleChangeDisY(void){


  float sinAngle;
  sinAngle = sin(convertToRad(flameAngle));
//  Serial.print(sinAngle);
//  Serial.print("  ");
//  Serial.print(averageDistance);
//  Serial.print("  ");
  return (float)(averageDistance) * sinAngle; 
  
  
}
/*********************************************************************************************/
void adjustFlamePos(void){
  xCoord = xCoord + 12 * cos(convertToRad(flameAngle));
  yCoord = yCoord + 12 * sin(convertToRad(flameAngle));  
}
/*********************************************************************************************/
float convertToRad(int angle){
  return (float)(PI/180) * (float)angle;
}




