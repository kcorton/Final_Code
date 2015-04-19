/*********************************************************************************************/
// Turn State machine  

// sets a global variable to determine whether to increment x or y or decrement x or y 

void turnStateMachine(int turnDeg){
  switch (drivingDirection) {
  case xPos:
    if (turnDeg > 0) {
      drivingDirection = yNeg;
    }
    if (turnDeg < 0) {
      drivingDirection = yPos;
    }
    break;
  case yPos:
    if (turnDeg > 0) {
      drivingDirection = xPos;
    }
    if (turnDeg < 0) {
      drivingDirection = xNeg;
    }  
    break;
  case xNeg:
    if (turnDeg > 0) {
      drivingDirection = yPos;
    }
    if (turnDeg < 0) {
      drivingDirection = yNeg;
    } 
    break;
  case yNeg: 
    if (turnDeg > 0) {
      drivingDirection = xNeg;
    }
    if (turnDeg < 0) {
      drivingDirection = xPos;
    }  
    break;
  }
}

/*********************************************************************************************/
// changeInDis function
// calculates based on the number of encoder tickes the changes in distance in inches and reutrns that

int changeInDis(void) {

  float leftDist = 0;
  float rightDist = 0;

  leftDist = inchesPerTick * leftCounter;
  rightDist = inchesPerTick * rightCounter;

  return (leftDist + rightDist) / 2 ; 

}

/*********************************************************************************************/
// update Location Function

//this function uses main global variable to determine which x and y coordinates to update and uses encoder values to update the new location of the robot

void updateLocation(void) {

  if (drivingDirection = xPos){
    XCoord = XCoord + changeInDis();
  }
  else if(drivingDirection = yPos) {
    YCoord = YCoord + changeInDis();
  }
  else if( drivingDirection = xNeg) {
    XCoord = XCoord - changeInDis();
  }
  else if(drivingDirection = yNeg) {
    YCoord = YCoord - changeInDis();
  }

}


