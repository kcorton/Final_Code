/*********************************************************************************************/
// Turn State machine  

// sets a global variable to determine whether to increment x or y or decrement x or y 

void turnStateMachine(int turnDeg){
  switch (drivingDirection) {
  case xPos:
    if (turnDeg > 0) {
      drivingDirection = yPos;
    }
    else if (turnDeg < 0) {
      drivingDirection = yNeg;
    }
    break;
  case yPos:
    if (turnDeg > 0) {
      drivingDirection = xNeg;
    }
    else if (turnDeg < 0) {
      drivingDirection = xPos;
    }  
    break;
  case xNeg:
    if (turnDeg > 0) {
      drivingDirection = yNeg;
    }
    else if (turnDeg < 0) {
      drivingDirection = yPos;
    } 
    break;
  case yNeg: 
    if (turnDeg > 0) {
      drivingDirection = xPos;
    }
    else if (turnDeg < 0) {
      drivingDirection = xNeg;
    }  
    break;
  default:
    Serial.println("HIT TURN STATE DEFAULT");
    lcd.println("ERROR 04");
    delay(5000);
  }
}

/*********************************************************************************************/
// changeInDis function
// calculates based on the number of encoder tickes the changes in distance in inches and reutrns that

int changeInDis(void) {

  float leftDist = 0;
  float rightDist = 0;

  leftDist = (float)inchesPerTick * leftCounter;
  rightDist = (float)inchesPerTick * rightCounter;

  return (leftDist + rightDist) / 2 ; 

}

/*********************************************************************************************/
// update Location Function

//this function uses main global variable to determine which x and y coordinates to update and uses encoder values to update the new location of the robot

void updateLocation(void) {

  if (drivingDirection == xPos){
    xCoord = xCoord + changeInDis();
  }
  else if(drivingDirection == yPos) {
    yCoord = yCoord + changeInDis();
  }
  else if( drivingDirection == xNeg) {
    xCoord = xCoord - changeInDis();
  }
  else if(drivingDirection == yNeg) {
    yCoord = yCoord - changeInDis();
  }
  
  leftCounter = 0;
  rightCounter = 0;
  lastLeftTicks = 0; 
  lastRightTicks = 0;

}

/*********************************************************************************************/
void printPosition(void) {
  
//  lcd.setCursor(0,0);
//  lcd.print(drivingDirection);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("(");
  lcd.setCursor(1, 0);
  lcd.print((int)xCoord);
  lcd.setCursor(4, 0);
  lcd.print(", ");
  lcd.setCursor(7, 0);
  lcd.print((int)yCoord);
  lcd.setCursor(10, 0);
  lcd.print(")");
  
//    lcd.setCursor(0, 0);
//  lcd.print("(");
//  lcd.setCursor(1, 0);
//  lcd.print((int)leftCounter);
//  lcd.setCursor(4, 0);
//  lcd.print(", ");
//  lcd.setCursor(7, 0);
//  lcd.print((int)rightCounter);
//  lcd.setCursor(10, 0);
//  lcd.print(")");
  
}


