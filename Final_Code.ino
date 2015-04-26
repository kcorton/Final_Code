


// Final Code Goes Here 

/*********************************************************************************************/
/* Necessary Include Files */

#include <Servo.h>
#include <TimerOne.h>
#include <L3G.h>
#include <Wire.h>

// Pin Values 
#define frontPingPin 27
#define sidePingPin 28
#define backPingPin 29
#define frontEchoPin 18
#define sideEchoPin 19
#define backEchoPin 26
#define fanPin 22
#define armMotorPin 4
#define armPotPin A4
#define leftMotorF 10
#define leftMotorB 9
#define leftEncoderB 8
#define leftEncoderA 2
#define rightMotorF 7
#define rightMotorB 6
#define rightEncoderB 5
#define rightEncoderA 3
#define firePin A1
#define fireServoPin 11
#define cliffSensorPin A2

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
#define candleDist 1 // the distance away from the candle when the robot will stop
#define frontWallDist 4  // the distance from the front wall when the robot should stop
#define desiredDist 4  //the desired distance between the robot and the wall
#define closeWallDist 6  // if a wall is within this value there is a close wall it should follow
#define BackFromCliffDist 5  // how far the robot will back up after seeing a cliff
#define forwardDisToTurnAboutWall 3 // how far the robot drives straight once it's lost the wall
#define ninetyDeg 90
#define negNinetyDeg -90
#define inchesPerTick .0032751103
#define turningSpeed 400


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

//Varables for Driveing Functions
int baseSpeed = 980; //encoder ticks per second
int leftSpeed = baseSpeed;
int rightSpeed = baseSpeed;

/*   Variables used by calcVelocity()   */
float speedStorage[2][5]; //stores five samples of the velocity from the last .5 seconds
int lastLeftTicks = 0; //holds the value of leftCounter from the previous run of calcVelocity() 
int lastRightTicks = 0;
int tempLeftTicks = 0; //holds the captured value of leftCounter
int tempRightTicks = 0;
int leftChange = 0;
int rightChange = 0;
float leftVelocity = 0; //encoder ticks per second
float rightVelocity = 0;
int nextIndex = 0; //the index currently being saved to in speedStorage
long lastTime = 0; //holds the time of the last running of calcVelocity()

/* Variables used by followWall() */
float distToWall = 0; //the actual distance between the robot and the wall
float wallError = 0;  //the difference between the above two
float Kw = 100;  //propotional multiplier which affects how much the wallError affects the speed
float velocityError = 0; //the difference between the speeds of the wheels
float Kv = 0.5;  //propotional multiplier which affects how much the velocityError affects the speed
volatile float accelTime;  //used as a multiplier to slow the robot's acceleration

//Variables for using the Sonar 
long frontPingTime = 0;
long sidePingTime = 0;
long backPingTime = 0;
volatile long frontEchoTime = 0;
volatile long sideEchoTime = 0;
volatile long backEchoTime = 0;
int tempEchoFront = 0;
int tempEchoSide = 0;
int tempEchoBack = 0;
int pingNext = frontSonar;

// variables used for turning with the gyro
L3G gyro;
int currGyroReading;
int mdps = 0;
float totalDegrees = 0;
long lastTurnCount = 0;
long lastTurnTime = 0;  
long currTurnTime = 0;
int offset = 0;

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
boolean waiting = false;

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

  // setup Cliff sensor
  pinMode(cliffSensorPin, INPUT_PULLUP);

  // setup Fan Motor
  armMotor.attach(armMotorPin);
  pinMode(armPotPin, INPUT);
  
  // setup Fan 
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin,LOW);

  // setup Flame Servo
  flameServo.attach(fireServoPin, 544, 2400);

  // sets up timer used for arm Function
  Timer1.initialize(100000); // interrupt every .1s or 10 times every second
  Timer1.attachInterrupt(timerISR);
  
  //Sonar pin declarations
  pinMode(frontPingPin,OUTPUT);
  pinMode(frontEchoPin,INPUT);
  pinMode(sidePingPin,OUTPUT);
  pinMode(sideEchoPin,INPUT);
  pinMode(backPingPin,OUTPUT);
  pinMode(backEchoPin,INPUT);
  
  //Motor pin declarations
  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);
  
  // I2C initialization
  Wire.begin();
  
  // Gyro Initializations
  calcGyroOffset();
  
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
  gyro.writeReg(L3G::CTRL_REG4, 0x20); // 2000 dps full scale
  gyro.writeReg(L3G::CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz  
  
  //Encoder interrupt initialization
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);
  
  //Sonar interrupt initialization
  attachInterrupt(5,frontSonarISR,CHANGE); 
  attachInterrupt(4,sideSonarISR,CHANGE);  
  
  //Ensures all Soonar pin pins are initialized to low
  digitalWrite(frontPingPin,LOW);
  digitalWrite(sidePingPin,LOW);
  digitalWrite(backPingPin,LOW);

  // set up interrups for drive Motors
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);  

}

void calcGyroOffset(void) {
  
  int i;
  long totalGyroReading = 0;
  
  for(i = 0; i < 1000; i++) {
    
    gyro.read();
    totalGyroReading += (int)gyro.g.z;
    
    delay(10);
    
  }
  
  Serial.println("OFFSET");
  offset = (float)totalGyroReading/1000;
  Serial.println(offset);
  
}

void loop() {
  
  //Serial.println(millis());
  
  ping(pingNext);    ////////Change which sonars are pinged based on what the main state in

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
  checkForCliff();


  switch (mazeState) {
  case followingWall:
    followWall();

    if(checkFrontDis(frontWallDist)){
      mazeState = seeingWallFront;
    }

    if(checkSideDisGreater(closeWallDist)){ 
    mazeState = loosingWall;
    }

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
    if(checkSideDisLess(closeWallDist)){
      mazeState = followingWall;
      lostWallState = 0;
    }
    break;
  case seeingCliff: // this state will be entered by an interrupt triggered by our line tracker **** KAREN THIS INTERRUPT NEEDS TO STOP ROBOT AND SET CLIFFSTATE = 0
    seenCliff();

    //once sonar can see a wall again 
    if(checkSideDisLess(closeWallDist)){
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
    turn(determineDriveDirection());
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
    turn(determineDriveDirection());

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









