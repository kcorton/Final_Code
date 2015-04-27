/*********************************************************************************************/
// madeItHome

void allDone(void) {
  stopAllDrive();

}

/*********************************************************************************************/
// Get Coordinates function 
// changes the global variables NextX and NextY to the next coordinates from the array of corrdinates created when traversing maze

void getCoordinates(void) {
  currentArrayRow--;
  nextXCoord = locationsArray[currentArrayRow][xCol];
  nextYCoord = locationsArray[currentArrayRow][yCol];  

}


/*********************************************************************************************/
// Drive To Next Coor function

// drives to next Coordinate, if there is a wall, keeping itself next to the wall, if no wall exists just uses encoders

void driveToNextCoor(void) {

  // if the sonar sees a close wall
  if(checkSideDisLess(closeWallDist)){
    followWall();
  }

  // if sensor sees no close wall 
  else {
    driveStraightForwardEnc();
  }

}

/*********************************************************************************************/
// Return Homoe Init Direction FUnction 
// sets the initial driving direction when robot is returning home and has found the wall again 
void returnHomeInitDirection(void){
  
  if (drivingDirection == xPos){
    drivingDirection = xNeg;
  }
  else if(drivingDirection == yPos) {
    drivingDirection = yNeg;
  }
  else if( drivingDirection == xNeg) {
    drivingDirection = xPos;
  }
  else if(drivingDirection == yNeg) {
    drivingDirection = yPos;
  }
  
}

/*********************************************************************************************/
// drive Home FUnction 
// Drives a given distance using encoders and maybe a wall if we have it

void driveHome(void){
  
}

/*********************************************************************************************/
// calculate Distance To Drive
// when the next coordinate is home this calculates how far the robot needs to drive
void calculateDistanceToDirve(void){
}
