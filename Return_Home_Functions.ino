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
  int disToDrive; 
  disToDrive = xCoord; 
  
  driveStraightDesDis(disToDrive);
  
  if(disTravComplete) {
    homeIsReached = true;
  }
  
}

