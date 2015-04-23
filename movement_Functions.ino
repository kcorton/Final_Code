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
    turnComplete = true;
  }
  else if (mainState == extinguishingFire) {
    stopAllDrive();
    turnComplete = true;
  }
  else if (mainState = extinguishingFire) { 
    stopAllDrive();
    turnComplete = true;
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

  if(disTraveledComplete(desDis)){ 
    stopAllDrive();
    disTravComplete = true;
  } 
}

/*********************************************************************************************/
// stop All Drive Function

// stops the drive motors from moving 

void stopAllDrive(void) {

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

}


