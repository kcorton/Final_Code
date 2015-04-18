
// Final Code Goes Here 

/*********************************************************************************************/
/* Necessary Include Files */

#include <Servo.h>
#include <TimerOne.h>

// Pin Values 
#define frontSonarPin1 1
#define frontSonarPin2 2
#define sideSonarPin1 1
#define sideSonarPin2 2
#define fanMotorPin 22
#define armMotorPin 4
#define armPotPin A4

// Main State machine States
#define findingFire 0
#define extinguishingFire 1
#define returningHome 2
#define madeItHome 3

//Finiding Fire State Machine States
#define followingWall 0
#define seeingWallFront 1
#define loosingWall 2
#define seeingCliff 3

// See Wall State Machine States
# define seeWallStoppingDrive 0
# define seeWallTurningState 1

// Lost Wall State Machine States
# define lostWallkeepDrivingStraight 0
# define lostWallTurning 1
# define lostWallDrivingStraight 2

// Seen Cliff State Machine States
# define seenCliffStoppingDrive 0
# define seenCliffBackingUp 1
# define SeenCliffTurningToStraight 2
# define SeenCliffBackOnCourse 3

//Extinguish Fire State Machine States
#define scaning 0
#define turningToFlame 1
#define drivingToCandle 2
#define activatingFan 3
#define checkingFlame 4
#define flameIsOut 5

//Return Home State Machine States
#define gettingCoordinates 0
#define determiningX 1
#define drivingToX 2
#define determiningY 3
#define drivingToY 4

//Fan Sweep State Machine
#define raisingArm 0
#define reachedTop 1
#define waitingAtTop 2
#define loweringArm 3

//Turning State Machine 
#define xPos 0
#define yPos 1
#define xNeg 2
#define yNeg 3


// Pre-defined Values and Distances
#define candleDist 1
#define frontWallDist 4
#define closeWallDist 6
#define BackFromCliffDist 3
#define forwardDisToTurnAboutWall 3
#define ninetyDeg 90


// Motor and Servo Declarations
Servo armMotor;

// Variables for Activate Fan Function   
int armTimePassed, armInitTime;
int armWaitTime = 100;
int highPos = 460;
int lowPos = 320;
long initTime = 0;

// State variables
int mainState = 0; 
int mazeState = 0; 
int seeWallState = 0; 
int lostWallState = 0; 
int cliffState = 0;
int extState = 0; 
int rtnState = 0; 
int armState = 0; 
int drivingDirection = 0; 

// Variables to keep track of where the robot is and has been
int XCoord = 0; 
int YCoord = 0; 
int nextXCoord = 0; 
int nextYCoord = 0; 

// boolean variables used in determining when to move to the next state
boolean turnComplete = false; 
boolean disTravComplete = false; 
boolean scanComplete = false;
boolean fanSweepComplete = false;
boolean hitTop = false;
boolean deactivateUpdateLocation = false;

// Timer Variables 
long countTime = 0;

void setup(){
  
  // need some way to determine which direction the robot is initially facing 
  
  Serial.begin(9600);

  // sets up timer used for arm Function
  Timer1.initialize(100000); // interrupt every .1s or 10 times every second
  Timer1.attachInterrupt(timerISR);
  
  // set up  for Fan Function
  armMotor.attach(armMotorPin);
  pinMode(armPotPin, INPUT);

}

void loop() {
 
  switch (mainState) {
  case findingFire: // navigating the maze looking for the fire
    findFire();
    break;
  case extinguishingFire: // fire was found and the robot is working to put it out
    extinguishFire();
    break;
  case returningHome: // fire is out and the robot is driving home
    returnHome();
    break;
  case madeItHome: // the robot has made it home
    allDone();
    break;
  }
}

/*********************************************************************************************/
// Timer ISR

void timerISR(){
  countTime++;
}

/*********************************************************************************************/
// FindFire Switch State

void findFire(void) {
  
  // updates the location of the robot from encoder values
  updateLocation();
  
  // looks for the fire 
  lookForFire();
  

  switch (mazeState) {
    case followingWall:
      followWall();
      
      if(checkFrontDis(frontWallDist)){
        mazeState = seeingWallFront;
      }
      
      // if the sonar sees a jump in the distance and is no longer next to a wall 
      mazeState = loosingWall;
      
      break;
    case seeingWallFront:
      seeWallFront();
      
      if(turnComplete){
        turnComplete = false;
        mazeState = followingWall;
        seeWallState = 0;
      }
      
      break;
    case loosingWall:
      lostWall();
      
      //once sonar can see a wall again
      if(checkSideDis(closeWallDist)){
        mazeState = followingWall;
        lostWallState = 0;
      }
      break;
    case seeingCliff: // this state will be entered by an interrupt triggered by our line tracker **** KAREN THIS INTERRUPT NEEDS TO STOP ROBOT AND SET CLIFFSTATE = 0
      seenCliff();
      
      //once sonar can see a wall again 
      if(checkSideDis(closeWallDist)){
        mazeState = followingWall;
        cliffState = 0;
      }
      
      break; 
    

  }

}

/*********************************************************************************************/
//Extinguish Fire Switch Statement

void extinguishFire(void){
  switch (extState) {
  case scaning: // scans full range of fire sensor
    scan();

    if(scanComplete) {
      extState = turningToFlame;
      scanComplete = false; 
    }

    break;

  case turningToFlame: // uses highest fire value's servo position and gyro to rotate robot towards flame
    turnTowardFlame();

    if(turnComplete){
        turnComplete = false;
        extState = drivingToCandle;
      }
     
    break;

  case drivingToCandle: 
    driveToCandle();

    if(checkFrontDis(candleDist)){
      extState = activatingFan;
      armState = raisingArm;
    }
    break;
  case activatingFan:
    activateFan();

    if(fanSweepComplete){ 
      extState = checkingFlame;
      fanSweepComplete = false;
    }

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

void returnHome(void) {

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

    if(turnComplete){
        turnComplete = false;
        rtnState = drivingToX;
      }
    

    break;
  case drivingToX:
    driveToX(); 

    if ( XCoord = nextXCoord){ 
      rtnState = determiningY; 
    }
    break;   
  case determiningY:
    determineY();

    if(turnComplete){
        turnComplete = false;
        rtnState = drivingToY;
      }
    

    break;
  case drivingToY:
    driveToY();  

    if(YCoord = nextYCoord){ 
      rtnState = gettingCoordinates; 
    }
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
// and saving a global variable with the servo position at the highest flame sensing
// this is used once the flame has been seen
// turns a global variable scanComplete to 1 once it has finished

void scan(void) {
  
  // if scan is complete 
  scanComplete = true;


}

/*********************************************************************************************/
// Turn Toward Flame function
// turns robot to the angle specified based on the variable changed in the scan function

void turnTowardFlame(void){

}

/*********************************************************************************************/
// Drive to Candle Function

void driveToCandle(void) {


  scan(); 
  turnTowardFlame();
  driveStraightForwardEnc();

}

/*********************************************************************************************/
// Drive Straight Forward Encoders function

//drives straight by ensuring both encoders have moved the same distance

void driveStraightForwardEnc(void) {

}

/*********************************************************************************************/
// Check Front Distance function 

// if the front distance sensors sees something  a certain distance away the function is true

boolean checkFrontDis(int desDis){

  if (getDis(frontSonarPin1, frontSonarPin2) <= desDis) { 
    return true; 
  }

  else { 
    return false; 
  }

}

/*********************************************************************************************/
// Check Side Distance function 

// if the side distance sensors sees something  a certain distance away the function is true

boolean checkSideDis(int desDis){

  if (getDis(sideSonarPin1, sideSonarPin2) <= desDis) { 
    return true; 
  }

  else { 
    return false; 
  }

}

/*********************************************************************************************/
// Get Front Distance Function  

// returns the value from the front distance sensor when called 

int getDis(int PinVal1, int PinVal2) {

}

/*********************************************************************************************/
// Activate Fan Function 

// Sweeps Fan between it's two extreme postions and checks if the fire is still detected 

void activateFan(void) { 

  // run fan between two extremes 
  digitalWrite(fanMotorPin, HIGH);
  armTimePassed = countTime - armInitTime;
  switch(armState){
    case raisingArm:
      armMotor.write(108);
      if(analogRead(armPotPin) > highPos){
        armState = reachedTop;
      }
      break;
    case reachedTop:
      initTime = countTime;
      armState = waitingAtTop;
      break;
    case waitingAtTop: 
      armMotor.write(100);
      if((armTimePassed-initTime) > armWaitTime){
      armState = loweringArm;
      }
      break;
    case loweringArm:
    armMotor.write(93);
    if (analogRead(armPotPin) <=  lowPos){
      armMotor.write(90);
      digitalWrite(fanMotorPin, LOW);
      fanSweepComplete = true;
    }
    break;
  }
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
// once the turn is complete all motors stop and the global variable turnComplete is set to 1

void turn(int turnDeg){
  // deactivate update Location 
  deactivateUpdateLocation = true;
  
  // if turn has been completed 
  stopAllDrive();
  storeLocation();
  turnStateMachine(turnDeg); 
  deactivateUpdateLocation = false; // might also need to reset encoder values here 
  turnComplete = true;

}

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
  case yPos:
  if (turnDeg > 0) {
    drivingDirection = xPos;
  }
  if (turnDeg < 0) {
    drivingDirection = xNeg;
  }  
 case xNeg:
  if (turnDeg > 0) {
    drivingDirection = yPos;
  }
  if (turnDeg < 0) {
    drivingDirection = yNeg;
  } 
  case yNeg: 
  if (turnDeg > 0) {
    drivingDirection = xNeg;
  }
  if (turnDeg < 0) {
    drivingDirection = xPos;
  }  
}
}

/*********************************************************************************************/
// Drive To Next Coor function

// drives to next Coordinate, if there is a wall, keeping itself next to the wall, if no wall exists just uses encoders

void driveToNextCoor(void) {
  
  // if sensor value is returning close wall value 
  driveStraightForwardEncSon();
  
  // if sensor sees no close wall 
  driveStraightForwardEnc();
  
}

/*********************************************************************************************/
// Look For Fire function

// This function is constantly polled when navigating the maze
// it rotates the servo and if it ever sees a "flame" changes the main State 

void lookForFire(void) {
  
  // if the fire has been found 
   mainState = extinguishingFire;
  
}

/*********************************************************************************************/
// update Location Function

//this function uses main global variable to determine which x and y coordinates to update and uses encoder values to update the new location of the robot

void updateLocation(void) {
  if (deactivateUpdateLocation) {
    // do nothing becuase we are turning and the encoder ticks should not be counted
  }
  else { 
    if (drivingDirection = xPos){
    
    }
    else if(drivingDirection = yPos) {
    
    }
    else if( drivingDirection = xNeg) {
    
    }
    else if(drivingDirection = yPos) {
    
    }
  }
  
}

/*********************************************************************************************/
// store Location Function
// this function saves the current location in an array to be used when the robot returns home

void storeLocation(void) {
  
}


/*********************************************************************************************/
// follow Wall Function
// this functions keeps the robot straight using the encoders and keeps it a certain distance from the wall 

void followWall(void) {
  driveStraightForwardEncSon();
  
}

/*********************************************************************************************/
// See Wall Front Function
// this function stops the robot, turns it 90 degress to the right, and updates which x and y variables should be updating as the robot drives

void seeWallFront(void) {
  
  switch (seeWallState){
    case seeWallStoppingDrive:
    stopAllDrive();
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
// the robot needs to stop driving, back up, and turns right 
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

/*********************************************************************************************/
// Drive Straight Desired Distance function

//drives straight by ensuring both encoders have moved the same distance until it reaches it's desired distance
// once this distance is reached it changes the gloabal variable disTravComplete to 1

void driveStraightDesDis(int desDis) {
  
  // if desired distance has been driven 
  stopAllDrive();
  disTravComplete = true; 

}

/*********************************************************************************************/
// stop All Drive Function

// stops the drive motors from moving 

void stopAllDrive(void) {
  
}

/*********************************************************************************************/
// Driving Straight Forward with Encoders and Sonar Function
// this function keeps the robot driving straight and a certain distance from the wall

void driveStraightForwardEncSon(void) {
  
}

