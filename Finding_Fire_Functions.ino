/*********************************************************************************************/
// store Location Function
// this function saves the current location in an array to be used when the robot returns home

void storeLocation(void) {
  
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
    turn(ninetyDeg);
    break;
  }
}

/*********************************************************************************************/
// lost Wall Function
// this drives the robot straight forward until it is the proper distance from the lost wall to turn left 
// it then drives forward and if it drives over a given distance it will turn left again and drive straight

void lostWall(void) {
  
  switch (lostWallState) {
    case lostWallkeepDrivingStraight: 
    driveStraightDesDis(forwardDisToTurnAboutWall);
      if(disTravComplete) {
        lostWallState = lostWallTurning;
        disTravComplete = false;
      }
      break;
      case lostWallTurning: 
      updateLocation();
      turn(-ninetyDeg);
      if(turnComplete){
        lostWallState = lostWallDrivingStraight;
        turnComplete = false;
      }
      break; 
     case lostWallDrivingStraight: 
     driveStraightForwardEnc();
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
      driveStraightDesDis(-BackFromCliffDist);
      if(disTravComplete) {
        cliffState = SeenCliffTurningToStraight;
        disTravComplete = false;
      }
      break;
    case SeenCliffTurningToStraight:
      updateLocation();
      turn(ninetyDeg);
      if(turnComplete){
        cliffState = SeenCliffBackOnCourse;
        turnComplete = false;
      }
      break;
    case SeenCliffBackOnCourse:
      driveStraightForwardEnc();
  }
}
