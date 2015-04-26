/*********************************************************************************************/
// madeItHome

void allDone(void) {

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

  if(checkSideDisLess(closeWallDist)){
  // if the sonar sees a close wall
  followWall();
  }
  
  else {
  // if sensor sees no close wall 
  driveStraightForwardEnc();
  }

}

/*********************************************************************************************/
// Angle Needed X
// returns the angle needed to turn to drive the robot to the correct x value when returning to the wall
// this will probably use whatever angle we originally turned or possibly the gyro

int angleNeededX(void){

}

/*********************************************************************************************/
// determine Distance X
// determines how far the robot needs to drive in the X direction and returns that value

int determineDistanceX(void) {

}


/*********************************************************************************************/
// Determine Distance Y function
// determines how far the robot needs to drive in the Y direction and returns that value

int determineDistanceY(void) {

}


/*********************************************************************************************/
// Determine Drive Dirrection Function
// determines which dirrection the robot needs to turn and returns that value in degrees
// clockwise = pos
// counterclockwise = neg
// also sets a global variable with how far it needs to drive in that direction

int determineDriveDirection(void) {

}

