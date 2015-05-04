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

    // Read current fire sensor value 
    flameVal = analogRead(firePin);

    flameServo.write(fireServoPos);

    // if the servo is increasing
    if(servoIncreasing) {
      // if the servo has not reached it's max value
      if(fireServoPos <= 110) {
        fireServoPos += 10; 
        flameServo.write(fireServoPos);     
      }
      // if it has reached it's max value
      else {
        servoIncreasing = 0;
      }
    }
    // if the servo is decreasing 
    else {
      // if it has not reached it's lowest value
      if(fireServoPos >= 10) {
        fireServoPos -= 10;
        flameServo.write(fireServoPos);
      }
      // if it has reached it's lowest value
      else {
        servoIncreasing = 1;
      }
    }
   
   // if the robot is not turning 
    if(robotIsTurning == false){
      /* If the flame is found */
      if(flameVal < 800) {
        /* Change to extiguish fire state */
        mainState = extinguishingFire;
        setInitialFlameAngle();
        stopAllDrive();
        updateLocation ();
        fireServoPos = 0; // reset the position
        lastFlameVal = 2000; // reset the last flame val
        flameServo.write(0); // write the servo back to reset position
        scanComplete = true; //Indicate the scan is complete
        //Serial.println("Fire Found");
      }
    }

    lastFireTimeCount = countTime; // reset the stored time variable

  }

}

/*********************************************************************************************/
// See Wall Front Function
// this function stops the robot, turns it 90 degress to the right, and updates which x and y variables should be updating as the robot drives

void seeWallFront(void) {

  switch (seeWallState){
  case seeWallStoppingDrive: // stop driving 
    stopAllDrive();
    updateLocation();
    seeWallState = seeWallTurningState;
    break;
  case seeWallTurningState: // turn negative ninety degrees
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
  case lostWallStopping: // stop driving for a second
    stopAllDrive(); 
    if ((countTime - tempTimer) >= 10){
      lostWallState = lostWallKeepDrivingStraight;
      accelTime = 0;
    }
    break;
  case lostWallKeepDrivingStraight: // drive straight a little to get away from the wall 
    driveStraightDesDis(forwardDisToTurnAboutWall);
    if(disTravComplete) {
      firstTimeThrough = true;
      lostWallState = lostWallTurning;
      disTravComplete = false;
    }
    break;
  case lostWallTurning: // turn about the wall
    updateLocation();
    turn(ninetyDeg);
    if(turnComplete){
      lostWallState = lostWallKeepDrivingStraight2;
      accelTime = 0;
      turnComplete = false;
    }
    break; 

  case lostWallKeepDrivingStraight2:  // drive straight to get around the penensula 
    driveStraightDesDis(forwardDisToTurnAboutWall2);
    if(disTravComplete) {
      firstTimeThrough = true;
      lostWallState = lostWallTurning2;
      disTravComplete = false;
    }
    break;

  case lostWallTurning2: // turn again to find the wall again 
    updateLocation();
    turn(ninetyDeg);
    if(turnComplete){
      lostWallState = lostWallDrivingStraight;
      turnComplete = false;
      accelTime = 0;
    }
    break; 

  case lostWallDrivingStraight: // drive straight looking for a wall to follow in the finding Fire state machine 
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
  case seenCliffStoppingDrive: // stop driving
    stopAllDrive();
    cliffState = seenCliffBackingUp; 
    break;
  case seenCliffBackingUp:  // back up from the cliff
    driveStraightDesDis(negBackFromCliffDist);
    if(disTravComplete) {
      if(mainState != returningHome){
        cliffState = SeenCliffTurningToStraight;
        disTravComplete = false;
        leftSpeed = baseSpeed;
        rightSpeed = baseSpeed;
      }
      else {
        cliffState = 0; 
        rtnState = determiningDriveDirrection;
        disTravComplete = false;
        leftSpeed = baseSpeed;
        rightSpeed = baseSpeed;
      }
    }
    break;
  case SeenCliffTurningToStraight: // turn back to drive straight 
    updateLocation();
    turn(negNinetyDeg);
    if(turnComplete){
      cliffState = SeenCliffBackOnCourse;
      turnComplete = false;
    }
    break;
  case SeenCliffBackOnCourse:  // back on course keep driving with the encoders 
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
// changes the proper state if a cliff is seen and stops driving
void checkForCliff(void){
  if(mainState == findingFire){

    if(digitalRead(cliffSensorPin) == HIGH){
      Serial.println("cliff!!!");
      lostWallState = 0;
      stopAllDrive();
      delay(500);
      mazeState = seeingCliff;
      cliffState = 0;
    }
  }
  else if(mainState == returningHome){
    if(digitalRead(cliffSensorPin) == HIGH){
      Serial.println("cliff!!!");
      stopAllDrive();
      delay(500);
      rtnState = seeingCliffReturning;

    }

  }
}
/*********************************************************************************************/
void setInitialFlameAngle(void){
  switch(drivingDirection){
  case xPos:
    flameAngle = 0;
    break;
  case xNeg:
    flameAngle = 180;
    break;
  case yPos:
    flameAngle = 90;
    break;
  case yNeg:
    flameAngle = -90;
    break;  
  }
}




