/*********************************************************************************************/
// Turn Function

// turns the robot about the turning center a given distance 
// pos - turns counterclockwise
// neg- turns clockwise
// once the turn is complete all motors stop and the global variable turnComplete is set to 1

void turn(int turnDeg){

  if (firstTimeThroughTurning){
    lastTurnTime = millis();
    firstTimeThroughTurning = false;
  }
  /* If enough time has passed */
  if(countTime - lastTurnCount >= 1) {

    currTurnTime = millis() - lastTurnTime;

    gyro.read();
    currGyroReading = (int)gyro.g.z;
    mdps = (currGyroReading - offset);
    mdps = (float)mdps * 0.07;

    totalDegrees += (float)mdps * ((float)currTurnTime/2000) * 2.1;

    if(turnDeg >= 0) {

      /* Write Changes to the motors */
      leftSpeed = -turningSpeed;
      rightSpeed = turningSpeed;
      updateMotors();

      // if the turn has been completed
      if(totalDegrees >= turnDeg) {
        turnComplete = true;
        accelTime = 0;
        stopAllDrive();
        lastTurnCount = 0;
        totalDegrees = 0;
        firstTimeThroughTurning = true;
        //        lastTurnTime = millis();                      ///////Call this before turn we think

        if (mainState == findingFire){
          storeLocation();
          turnStateMachine(turnDeg);  
        }
        else if (mainState == extinguishingFire) {
          // do nothing for now
        }
        else if (mainState = extinguishingFire) { 
          turnStateMachine(turnDeg);
        }
      }
    }

    else if(turnDeg < 0) {

      /* Write Changes to the motors */
      leftSpeed = turningSpeed;
      rightSpeed = -turningSpeed;
      updateMotors();

      if(totalDegrees <= turnDeg) {
        turnComplete = true;
        accelTime = 0;
        stopAllDrive();
        lastTurnTime = 0;
        totalDegrees = 0;
        firstTimeThroughTurning = true;
        //        lastTurnTime = millis();

        if (mainState == findingFire){
          storeLocation();
          turnStateMachine(turnDeg);  
        }
        else if (mainState == extinguishingFire) {
          // do nothing for now
        }
        else if (mainState = extinguishingFire) { 
          // do nothing for now
        }
      }
    }

//    Serial.println(totalDegrees);

    lastTurnTime = millis();
    lastTurnCount = countTime;

  }

}

/*********************************************************************************************/
// Dis Traveled Complete Function
// returns true when the correct distance has been traveled

boolean disTraveledComplete(int desDis) {
  if(firstTimeThrough){
    temporaryLeftCounter = leftCounter; 
    temporaryRightCounter = rightCounter;
    firstTimeThrough = false;
  }
  if (desDis > 0){
    if ( (leftCounter - temporaryLeftCounter) >= desDis){
      if ((rightCounter - temporaryRightCounter) >= desDis){
        return true; 
      }
    }
    else {
      return false;
    }
  }
  else if (desDis < 0){
    if ( (leftCounter - temporaryLeftCounter) <= desDis){
      if ((rightCounter - temporaryRightCounter) <= desDis){
        return true; 
      }
    }
    else {
      return false;
    }
  }

}

/*********************************************************************************************/
// Drive Straight Desired Distance function

//drives straight by ensuring both encoders have moved the same distance until it reaches it's desired distance
// once this distance is reached it changes the gloabal variable disTravComplete to 1

void driveStraightDesDis(int desDis) {

  if(desDis > 0){

    driveStraightForwardEnc();

    if(disTraveledComplete(desDis)){ 
      stopAllDrive();
      disTravComplete = true;
    } 
  }
  else if(desDis < 0){
    driveStraightBackwardsEnc();

    if(disTraveledComplete(desDis)){ 
      stopAllDrive();
      disTravComplete = true;
    }   
  }

}

/*********************************************************************************************/
// stop All Drive Function

// stops the drive motors from moving 

void stopAllDrive(void) {
  leftSpeed = 0; 
  rightSpeed = 0; 
  updateMotors();

}


/*********************************************************************************************/
// Drive Straight Forward Encoders function

//drives straight by ensuring both encoders have moved the same distance

void driveStraightForwardEnc(void) {
  Kw = 0;
  followWall();

}

/*********************************************************************************************/
// Drive Straight Forward Encoders function

//drives straight by ensuring both encoders have moved the same distance

void driveStraightBackwardsEnc(void) {
  leftSpeed = -baseSpeed;
  rightSpeed = -baseSpeed;
  updateMotors();
  

}

/*********************************************************************************************/
// follow Wall Function
// this functions keeps the robot straight using the encoders and keeps it a certain distance from the wall 

void followWall(void) {

  calcVelocity();
  //calculate sonar error
  distToWall = getDis(sideSonar);
  wallError = desiredDist - distToWall;
  //calculate velocity error
  velocityError = getRightVeloc() - getLeftVeloc();
  //calculate speed based on the errors (proportional control)
  leftSpeed = accelTime * (baseSpeed + (Kw * wallError) + (Kv * velocityError));
  rightSpeed = accelTime * (baseSpeed - (Kw * wallError) - (Kv * velocityError));
  //set the motor speeds
  updateMotors();

}

//==========================================================================
//calculates the average velocity of the robot in encoder ticks per second and
// stores the values in a 2x5 array with circular indexing. The left velocity
// is stored in row 0 and the right is stored in row 1.
//assumes the timer triggers every .1 s, resulting in this function storing
// the last .5 seconds of velocity data
void calcVelocity(){
  if(countTime - lastTime > 0){ // true if the timer went off since the last running of calcVelocity()
    lastTime = countTime;
    //save the current number of ticks so they dont change
    // while calculations are being made
    tempLeftTicks = leftCounter;
    tempRightTicks = rightCounter;

    //calculate the change in ticks since the last time this function ran
    leftChange = tempLeftTicks - lastLeftTicks;
    rightChange = tempRightTicks - lastRightTicks;

    //calculate the velocity (assumes this function runs every .1 seconds
    leftVelocity = leftChange/.1;
    rightVelocity = rightChange/.1;

    //set the new initial ticks in preparation for the next run
    lastLeftTicks = tempLeftTicks;
    lastRightTicks = tempRightTicks; 

    //increment the storing index
    if(nextIndex >= 4){
      nextIndex = 0;  
    }
    else{
      nextIndex++;
    }

    //store the left and right velocities in a 2x5 array
    speedStorage[0][nextIndex] = leftVelocity;
    speedStorage[1][nextIndex] = rightVelocity; 
  }
}
//==========================================================================
//calculates the average velocity of the left wheel in encoder ticks per second
// using the data stored in speedStorage[0]
float getLeftVeloc(){
  int sum = 0;
  //sum the stored velocities
  for(int i = 0; i < 5; i++){
    sum += speedStorage[0][i];  
  }
  //calculate and return the average
  return sum/5;
}
//==========================================================================
//calculates the average velocity of the right wheel in encoder ticks per second
// using the data stored in speedStorage[1]
float getRightVeloc(){
  int sum = 0;
  //sum the stored velocities
  for(int i = 0; i < 5; i++){
    sum += speedStorage[1][i];  
  }
  //calculate and return the average
  return sum/5;
}

//==========================================================================
//determines whether the wheels should spin forwards or backwards based on
// the sign, then maps the speeds, which are originally in encoder ticks
// per second, to the corresponding analog write value
void updateMotors(){
  if(leftSpeed >= 0){  //drive forward
    analogWrite(leftMotorF,mapSpeed(leftSpeed));
    analogWrite(leftMotorB,0);
  }
  else{                //drive backward
    analogWrite(leftMotorF,0);
    analogWrite(leftMotorB,(mapSpeed(leftSpeed)));
  }
  if(rightSpeed >= 0){ //drive forward
    analogWrite(rightMotorF,mapSpeed(rightSpeed));
    analogWrite(rightMotorB,0);
  }
  else{                //drive backward
    analogWrite(rightMotorF,0);
    analogWrite(rightMotorB,(mapSpeed(rightSpeed)));
  }
}
//==========================================================================
//returns an analogWrite value (0 to 255) corresponding to the input speed
//Note: ignores the sign of inSpeed since negatives are already taken care of
//      of in updateMotors()
int mapSpeed(int inSpeed){
  //get the absolute value of inSpeed
  if(inSpeed < 0){
    inSpeed *= -1;  
  }
  //convert to an analogWrite value (0 to 255)
  return ( (float)inSpeed/9.8 );
}







