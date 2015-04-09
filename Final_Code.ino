// Final Code Goes Here 

/*********************************************************************************************/
/* Necessary Include Files */


#define findingFire 0
#define extinguishingFire 1
#define returningHome 2
#define madeItHome 3

#define scaning 0
#define turningToFlame 1
#define drivingToCandle 2
#define activatingFan 3
#define checkingFlame 4
#define flameIsOut 5

#define gettingCoordinates 0
#define determiningX 1
#define drivingToX 2
#define determiningY 3
#define drivingToY 4

#define candleDist 1
#define ninetyDeg 90


// State variables
int mainState = 0; 
int mazeState = 0; 
int extState = 0; 
int rtnState = 0; 

int XCoord = 0; 
int YCoord = 0; 
int nextXCoord = 0; 
int nextYCoord = 0; 


void setup(){


}

void loop() {
  switch (mainState) {
  case findingFire:
    findFire();
    break;
  case extinguishingFire:
    extinguishFire();
    break;
  case returningHome:
    returnHome();
    break;
  case madeItHome: 
    allDone();
    break;
  }
}


/*********************************************************************************************/
// FindFire Switch State

void findFire(void) {

  switch (mazeState) {

  }

}

/*********************************************************************************************/
//Extinguish Fire Switch Statement

void extinguishFire (void){
  switch (extState) {
  case scaning: // scans full range of fire sensor
    scan();

    // if scan is complete 
    extState = turningToFlame;

    break;

  case turningToFlame: // uses highest fire value's servo position and gyro to rotate robot towards flame
    turnTowardFlame();

    //if turn is complete
    extState = drivingToCandle; 

    break;

  case drivingToCandle: 
    driveToCandle();

    if(checkFrontDis(candleDist)){
      extState = activatingFan ;
    }
    break;
  case activatingFan:
    activateFan();

    //If fan sweep is complete 
    extState = checkingFlame;

    break;

  case checkingFlame: 
    scan();

    // if scan returns interrupt on Fire Sensor 
    extState = activatingFan; 

    // else 

    extState = flameIsOut ; 
  case flameIsOut: 
    reportFlame(); 
    mainState = returningHome;
    break; 


  }
}

/*********************************************************************************************/
// Return Home Switch Statement

void returnHome (void) {

  switch (rtnState) {
  case gettingCoordinates:
    getCoordinates();

    // if getCoordinate return a next coordinate 
    rtnState = determiningX;

    //if getCoordinate returns 0 for no next coordinate
    mainState = madeItHome;

    break;
  case determiningX:
    determineX();

    //If turn has been completed
    rtnState = drivingToX;

    break;
  case drivingToX:
    driveToX(); 

    // when X Coordinate has been reached 
    rtnState = determiningY; 
    break;   
  case determiningY:
    determineY();

    //If turn has been completed
    rtnState = drivingToY;

    break;
  case drivingToY:
    driveToY();  

    // when Y Coordinate has been reached 
    rtnState = gettingCoordinates; 
    break; 
  }

}


/*********************************************************************************************/
// madeItHome

void allDone(void) {

}

/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a variable with the servo position at the highest flame sensing

void scan (void) {


}

/*********************************************************************************************/
// Turn Toward Flame function
// turns robot to the angle specified based on the variable changed in the scan function

void turnTowardFlame(void){

}

/*********************************************************************************************/
// Drive to Candle Function

void driveToCandle (void) {


  scan(); 
  turnTowardFlame();
  driveStraightForward();

}

/*********************************************************************************************/
// Drive Straight Frward function

//drives straight by ensuring both encoders have moved the same distance

void driveStraightForward(void) {

}

/*********************************************************************************************/
// Check Front Distance function 

// if the front distance sensors sees something  a certain distance away the function is true

boolean checkFrontDis(int desDis){

  if (getFrontDis() <= desDis) { 
    return true; 
  }

  else { 
    return false; 
  }

}

/*********************************************************************************************/
// Get Front Distance Function  

// returns the value from the front distance sensor when called 

int getFrontDis(void) {

}

/*********************************************************************************************/
// Activate Fan Function 

// Sweeps Fan between it's two extreme postions and checks if the fire is still detected 

void activateFan (void) { 

  // run fan between two extremes 


}

/*********************************************************************************************/
// Report Flame Function 

// reports the location of the flame 

void reportFlame(void) {

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
// Turn Function

// turns the robot about the turning center a given distance 
// pos - turns clockwise
// neg- turns counterclockwise

void turn(int turnDeg){

}

/*********************************************************************************************/
// Drive To Next Coor function

// drives to next Coordinate, if there is a wall, keeping itself next to the wall, if no wall exists just uses encoders

void driveToNextCoor (void) {
  
}








