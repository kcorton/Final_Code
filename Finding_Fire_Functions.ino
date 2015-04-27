/*********************************************************************************************/
// store Location Function
// this function saves the current location in an array to be used when the robot returns home

void storeLocation(void) {
  currentArrayRow++; 
  locationsArray[currentArrayRow][xCol] = xCoord;
  locationsArray[currentArrayRow][yCol] = yCoord;

}

/*********************************************************************************************/
// Look For Fire function


// This function is constantly polled when navigating the maze
// it rotates the servo and if it ever sees a "flame" changes the main State 

void lookForFire(void) {


  int flameVal;

  if(countTime - lastFireTimeCount >= 1) {

    Serial.println("lookForFire");
    /* Read current fire sensor value */
    flameVal = analogRead(firePin);
    Serial.println(flameVal);
    //Serial.println(fireServoPos);

    flameServo.write(fireServoPos);

    if(servoIncreasing) {
      if(fireServoPos <= 170) {
        fireServoPos += 10; 
        flameServo.write(fireServoPos);     
      }
      else {
        servoIncreasing = 0;
      }
    }
    else {
      if(fireServoPos >= 10) {
        fireServoPos -= 10;
        flameServo.write(fireServoPos);
      }
      else {
        servoIncreasing = 1;
      }
    }


    /* If the flame is found */
    if(flameVal < 300) {
      /* Change to extiguish fire state */
      mainState = extinguishingFire;
      stopAllDrive();
      fireServoPos = 0; // reset the position
      lastFlameVal = 2000; // reset the last flame val
      flameServo.write(0); // write the servo back to reset position
      scanComplete = true; //Indicate the scan is complete
      Serial.println("Fire Found");
    }

    lastFireTimeCount = countTime; // reset the stored time variable

  }

}

/*********************************************************************************************/
// See Wall Front Function
// this function stops the robot, turns it 90 degress to the right, and updates which x and y variables should be updating as the robot drives

void seeWallFront(void) {

  switch (seeWallState){
  case seeWallStoppingDrive:
    stopAllDrive();
    updateLocation();
    seeWallState = seeWallTurningState;
    break;
  case seeWallTurningState:
    turn(negNinetyDeg);
    break;
  default:
    Serial.println("HIT SEE WALL FUNCTION DEFAULT");
    lcd.print("ERROR 07");
    delay(5000);
    break;
  }
}

/*********************************************************************************************/
// lost Wall Function
// this drives the robot straight forward until it is the proper distance from the lost wall to turn left 
// it then drives forward and if it drives over a given distance it will turn left again and drive straight

void lostWall(void) {

  switch (lostWallState) {
  case lostWallStopping:
    stopAllDrive(); 
    if ((countTime - tempTimer) >= 10){
      lostWallState = lostWallKeepDrivingStraight;
      accelTime = 0;
    }
    break;
  case lostWallKeepDrivingStraight: 
    driveStraightDesDis(forwardDisToTurnAboutWall);
    if(disTravComplete) {
      firstTimeThrough = true;
      lostWallState = lostWallTurning;
      disTravComplete = false;
    }
    break;
  case lostWallTurning: 
    updateLocation();
    turn(ninetyDeg);
    if(turnComplete){
      lostWallState = lostWallKeepDrivingStraight2;
      accelTime = 0;
      turnComplete = false;
    }
    break; 

  case lostWallKeepDrivingStraight2: 
    driveStraightDesDis(forwardDisToTurnAboutWall2);
    if(disTravComplete) {
      firstTimeThrough = true;
      lostWallState = lostWallTurning2;
      disTravComplete = false;
    }
    break;

  case lostWallTurning2: 
    updateLocation();
    turn(ninetyDeg);
    if(turnComplete){
      lostWallState = lostWallDrivingStraight;
      turnComplete = false;
      accelTime = 0;
    }
    break; 

  case lostWallDrivingStraight: 
    driveStraightForwardEnc();
    break;

  default:
    Serial.println("HIT LOST WALL DEFAULT");
    lcd.print("ERROR 08");
    delay(5000);
    break;
  }

}

/*********************************************************************************************/
// seen Cliff Function
// the robot needs to stop driving, back up, and s right 
// then it keeps driving straight until it finds the wall again

void seenCliff(void) {   

  switch(cliffState){
  case seenCliffStoppingDrive: 
    stopAllDrive();
    cliffState = seenCliffBackingUp; 
    break;
  case seenCliffBackingUp:
    driveStraightDesDis(negBackFromCliffDist);
    if(disTravComplete) {
      cliffState = SeenCliffTurningToStraight;
      disTravComplete = false;
      leftSpeed = baseSpeed;
      rightSpeed = baseSpeed;
    }
    break;
  case SeenCliffTurningToStraight:
    updateLocation();
    turn(negNinetyDeg);
    if(turnComplete){
      cliffState = SeenCliffBackOnCourse;
      turnComplete = false;
    }
    break;
  case SeenCliffBackOnCourse:
    driveStraightForwardEnc();
    break;
  default:
    Serial.println("HIT SEEN CLIFF DEFAULT");
    lcd.println("ERROR 09");
    delay(5000);
  }
}


/*********************************************************************************************/
// checkForCliff
// changes the state
void checkForCliff(void){

  if(digitalRead(cliffSensorPin) == HIGH){
    Serial.println("cliff!!!");
    lostWallState = 0;
    stopAllDrive();
    delay(500);
    mazeState = seeingCliff;
    cliffState = 0;
  }

}




