
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
#define leftMotorF 6
#define leftMotorB 11
#define leftEncoderB 10
#define leftEncoderA 2
#define rightMotorF 9
#define rightMotorB 8
#define rightEncoderB 7
#define rightEncoderA 3
#define firePin A1

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
# define lostWallKeepDrivingStraight 0
# define lostWallTurning 1
# define lostWallKeepDrivingStraight2 2
# define lostWallTurning2 3
# define lostWallDrivingStraight 4

// Seen Cliff State Machine States
# define seenCliffStoppingDrive 0
# define seenCliffBackingUp 1
# define SeenCliffTurningToStraight 2
# define SeenCliffBackOnCourse 3

//Extinguish Fire State Machine States
#define drivingToCandle 0
#define activatingFan 1
#define checkingFlame 2
#define flameIsOut 3

// Driving to Candle State Machine States
#define scanning 0
#define turningToCandle 1
#define drivingForTime 2
#define updatingLocation 3

//Return Home State Machine States
#define gettingToWallTurningX 0
#define gettingBackToWallX 1
#define gettingToWallTurningY 2
#define gettingBackToWallY 3
#define gettingCoordinates 4
#define determiningDriveDirrection 5
#define drivingToCoordinate 6

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

// Sonar Names 
#define frontSonar 0
#define sideSonar 1
#define backSonar 2

// Array Columns 
#define xCol 0
#define yCol 1

// Pre-defined Values and Distances
#define candleDist 1
#define frontWallDist 4
#define closeWallDist 6
#define BackFromCliffDist 3
#define forwardDisToTurnAboutWall 3
#define ninetyDeg 90
#define inchesPerTick .0032751103


// Motor and Servo Declarations
Servo armMotor;
Servo flameServo;

// Variables for Activate Fan Function   
int armTimePassed, armInitTime;
int armWaitTime = 100;
int highPos = 460;
int lowPos = 320;
long initTime = 0;

// Variables for Fire Sensor Functions
int servoIncreasing = 0;
int fireServoPos = 0;
long lastFireTimeCount = 0;
int firePosition; 
int lastFlameVal = 2000;

// Variables for Return Home
int disToNextCoor = 0;

// Variables to keep track of where the robot is and has been
int xCoord = 0; 
int yCoord = 0; 
int nextXCoord = 0; 
int nextYCoord = 0; 
int locationsArray[15][2] = {0};
int currentArrayRow = 0;

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
int driveToCandleState = 0;

// boolean variables used in determining when to move to the next state
boolean turnComplete = false; 
boolean disTravComplete = false; 
boolean scanComplete = false;
boolean fanSweepComplete = false;
boolean hitTop = false;

// Interrupt Variables
volatile long countTime = 0;
volatile long rightCounter = 0;
volatile long leftCounter = 0;

void setup(){

  Serial.begin(9600);

  // need some way to determine which direction the robot is initially facing 

  // setup drive motors
  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);

  // setup Fan Motor
  armMotor.attach(armMotorPin);
  pinMode(armPotPin, INPUT);

  // setup Flame Servo
  flameServo.attach(9, 544, 2400);

  // sets up timer used for arm Function
  Timer1.initialize(100000); // interrupt every .1s or 10 times every second
  Timer1.attachInterrupt(timerISR);

  // set up interrups for drive Motors
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);  

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
// FindFire Switch State

void findFire(void) {

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
  case gettingToWallTurningX: 
    turn(angleNeededX());
    if (turnComplete) {
      turnComplete = false; 
      rtnState = gettingBackToWallX;
    }
    break;
  case gettingBackToWallX:
    driveStraightDesDis(determineDistanceX());
    if (disTravComplete){
      disTravComplete = false;
      rtnState = gettingToWallTurningY;
      updateLocation();
    }
    break;
  case gettingToWallTurningY: 
    turn(determineDriveDirrection());
    if (turnComplete) {
      turnComplete = false; 
      rtnState = gettingBackToWallY;
    }
    break;    
  case gettingBackToWallY:
    driveStraightDesDis(determineDistanceY());
    if (disTravComplete){
      disTravComplete = false;
      rtnState = gettingCoordinates;
      updateLocation();
    }
    break;    
  case gettingCoordinates:
    getCoordinates();

    // if getCoordinate return a next coordinate 
    rtnState = determiningDriveDirrection;

    //if getCoordinate returns 0 for no next coordinate
    mainState = madeItHome;

    break;
  case determiningDriveDirrection:
    turn(determinedriveDirection());

    if(turnComplete){
      turnComplete = false;
      rtnState = drivingToCoordinate;
    }
    break;
  case drivingToCoordinate:
    driveToNextCoor(); 

    if(checkFrontDis(frontWallDist)){
      stopAllDrive();
      updateLocation();
      rtnState = gettingCoordinates;
    }
    if(disTraveledComplete(disToNextCoor)) { 
      stopAllDrive();
      updateLocation();
      rtnState = gettingCoordinates;
    }

    break;   

  }
}









