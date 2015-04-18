/*********************************************************************************************/
// madeItHome

void allDone(void) {

}

/*********************************************************************************************/
// Get Coordinates function 

// changes the global variables NextX and NextY to the next coordinates from the array of corrdinates created when traversing maze

void getCoordinates(void) {

}

/*********************************************************************************************/
// Determine X function

// uses the current and next X value to determine which direction pos or neg X to drive and turns that way

void determineX(void) {

  // if needs to turn left 
  turn( - ninetyDeg);

  // if needs to turn right 
  turn ( ninetyDeg);

}

/*********************************************************************************************/
// Driving to X function

//Drives to next X coordinate, if there is a wall, keeping itself next to the wall, if no wall exists just uses encoders

void driveToX(void) {
  driveToNextCoor();
}

/*********************************************************************************************/
// Determine Y function

// uses the current and next Y value to determine which direction pos or neg Y to drive and turns that way

void determineY(void) {

  // if needs to turn left 
  turn( - ninetyDeg);

  // if needs to turn right 
  turn ( ninetyDeg);

}

/*********************************************************************************************/
// Driving to Y function

//Drives to next Y coordinate, if there is a wall, keeping itself next to the wall, if no wall exists just uses encoders

void driveToY(void) {
  driveToNextCoor();
}


/*********************************************************************************************/
// Drive To Next Coor function

// drives to next Coordinate, if there is a wall, keeping itself next to the wall, if no wall exists just uses encoders

void driveToNextCoor(void) {
  
  // if sensor value is returning close wall value 
  followWall();
  
  // if sensor sees no close wall 
  driveStraightForwardEnc();
  
}


