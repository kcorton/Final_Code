/*********************************************************************************************/
// Turn Function

// turns the robot about the turning center a given distance 
// pos - turns clockwise
// neg- turns counterclockwise
// once the turn is complete all motors stop and the global variable turnComplete is set to 1

void turn(int turnDeg){
  

  // if turn has been completed 
  if (mainState == findingFire){
    stopAllDrive();
    storeLocation();
    turnStateMachine(turnDeg);  
  }
  else if (mainState == extinguishingFire) {
    stopAllDrive();
  }
  else if (mainState = extinguishingFire) { 
    stopAllDrive();
  }
}

/*********************************************************************************************/
// Dis Traveled Complete Function
// returns true when the correct distance has been traveled

boolean disTraveledComplete(int desiredDis) {

}

/*********************************************************************************************/
// Drive Straight Desired Distance function

//drives straight by ensuring both encoders have moved the same distance until it reaches it's desired distance
// once this distance is reached it changes the gloabal variable disTravComplete to 1

void driveStraightDesDis(int desDis) {
  
  driveStraightForwardEnc();

  if(disTraveledComplete(desDis)){ 
    stopAllDrive();
    disTravComplete = true;
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
//  Serial.print(leftSpeed);
//  Serial.print("  ");
//  Serial.print(rightSpeed);
//  Serial.print("  ");
//  Serial.print(getLeftVeloc());
//  Serial.print("  ");
//  Serial.println(getRightVeloc());
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


