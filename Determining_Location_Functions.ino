/*********************************************************************************************/
// Turn State machine  
// based on the current direction the robot is driving and how it turned 
// sets a global variable to determine the new driving Direction 

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
// this function uses main global variable to determine which x and y coordinates to update 
// uses encoder values to update the new location of the robot and clears the encoder values

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
// Print Position Function 
// prints the x and y coord variables to the LCD screen 

void printPosition(void) {

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
  
}

/*********************************************************************************************/
// Updating Angle Drive Location 
// this updates the x and y corrdinates based on the angle the robot was driving at towards the candle

void updateAngleDriveLocation(void){
  averageDist();
  xCoord = xCoord + candleChangeDisX();
  yCoord = yCoord + candleChangeDisY();
  

  leftCounter = 0;
  rightCounter = 0;
  lastLeftTicks = 0; 
  lastRightTicks = 0;

}

/*********************************************************************************************/
// Average Distance Function 
// calculates the average distance driven by the two encoders 
void averageDist(void){
  float leftDist = 0;
  float rightDist = 0;

  leftDist = inchesPerTick * leftCounter;
  rightDist = inchesPerTick * rightCounter;
  
  averageDistance = ((leftDist + rightDist) / 2);
}

/*********************************************************************************************/
// Candle Change Dis X 
// returns the change in X based on the angle turned and encoder ticks
float candleChangeDisX(void){
  
  float cosAngle;
  cosAngle = cos(convertToRad(flameAngle));
  return (float)(averageDistance) * cosAngle;
  
}
/*********************************************************************************************/
// Candle Change Dis Y 
// returns the change in Y based on the angle turned and encoder ticks
float candleChangeDisY(void){


  float sinAngle;
  sinAngle = sin(convertToRad(flameAngle));
  return (float)(averageDistance) * sinAngle; 
  
  
}
/*********************************************************************************************/
// Adjust Flame Position Function 
// adds the extra distance from the center of the robot to the center of the candle
void adjustFlamePos(void){
  xCoord = xCoord + 12 * cos(convertToRad(flameAngle));
  yCoord = yCoord + 12 * sin(convertToRad(flameAngle));  
}
/*********************************************************************************************/
// Convert To Radians Function
// converts an angle in degrees to radians

float convertToRad(int angle){
  return (float)(PI/180) * (float)angle;
}
