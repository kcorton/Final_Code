// Final Code Goes Here 

/*********************************************************************************************/
/* Necessary Include Files */

#include <Servo.h>
#include <TimerOne.h>
#include <L3G.h>
#include <Wire.h>
#include <LiquidCrystal.h>

// Pin Values 
#define frontPingPin 27
#define sidePingPin 28
#define backPingPin 29
#define frontEchoPin 18
#define sideEchoPin 25
#define backEchoPin 26
#define fanPin 22
#define armMotorPin 4
#define armPotPin A3
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
#define ledArrayPin 23
#define startButtonPin 24

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
#define seeWallStoppingDrive 0
#define seeWallTurningState 1

// Lost Wall State Machine States
#define lostWallStopping 0
#define lostWallKeepDrivingStraight 1
#define lostWallTurning 2
#define lostWallKeepDrivingStraight2 3
#define lostWallTurning2 4
#define lostWallDrivingStraight 5

// Seen Cliff State Machine States
#define seenCliffStoppingDrive 0
#define seenCliffBackingUp 1
#define SeenCliffTurningToStraight 2
#define SeenCliffBackOnCourse 3

//Extinguish Fire State Machine States
#define initialScanning 0
#define drivingToCandle 1
#define activatingFan 2
#define checkingFlame 3
#define flameIsOut 4

// Driving to Candle State Machine States
#define scanning 0
#define turningToCandle 1
#define drivingForTime 2
#define updatingLocation 3

//Return Home State Machine States
#define gettingToWallTurning180 0
#define gettingBackToWallDrive 1
#define straighteningRobot 2
#define turningOnce 3
#define gettingCoordinates 4
#define drivingToCoordinate 5
#define DrivingToTurnAboutWall 6
#define determiningDriveDirrection 7
#define drivingToHome 8

//Fan Sweep State Machine
#define armUp 0
#define armDown 1

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
#define candleDist 3 // the distance away from the candle when the robot will stop
#define frontWallDist 5  // the distance from the front wall when the robot should stop
#define desiredDist 4  //the desired distance between the robot and the wall
#define closeWallDist 6  // if a wall is within this value there is a close wall it should follow
#define negBackFromCliffDist -500  //(5 inches) how far the robot will back up after seeing a cliff in encoder t
#define forwardDisToTurnAboutWall2 5000 
#define forwardDisToTurnAboutWall 1500//(2 inches) how far the robot drives straight once it's lost the wall in encoder ticks
#define ninetyDeg 90
#define negNinetyDeg -90
#define pullAUiey 180
#define inchesPerTick .0066401062 
#define turningSpeed 400
#define wallProportionalVal 100

//LCD setup
LiquidCrystal lcd(13, 12, 17, 16, 15, 14);

// Motor and Servo Declarations
Servo armMotor;
Servo flameServo;

// Variables for Activate Fan Function   
//int armTimePassed, armInitTime;
//int armWaitTime = 5;
//int highPos = 390;
//int midPos = 350;
//int lowPos = 320;
//long initTime = 0;
int armWrite, armPos;
volatile int pidOut;
#define Kp 2
#define Kd 1
#define Ki 0

//Variable for Driving To Candle
long initTimeCandleDrive = 0;
int candleTurn = 0;

// Variables for Fire Sensor Functions
int servoIncreasing = 0;
int fireServoPos = 0;
long lastFireTimeCount = 0;
int firePosition; 
int lastFlameVal = 2000;

// Variables for Return Home
int disToNextCoor = 0;

//Varables for Driveing Functions
int baseSpeed = 700; //encoder ticks per second
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
float Kw = wallProportionalVal;  //propotional multiplier which affects how much the wallError affects the speed
float velocityError = 0; //the difference between the speeds of the wheels
float Kv = 0.5;  //propotional multiplier which affects how much the velocityError affects the speed
volatile float accelTime;  //used as a multiplier to slow the robot's acceleration

//Variables for using the Sonar 
volatile long frontPingTime = 0;
volatile long sidePingTime = 0;
long backPingTime = 0;
volatile long frontEchoTime = 0;
volatile long sideEchoTime = 0;
volatile long backEchoTime = 0;
volatile int tempEchoFront = 0;
volatile int tempEchoSide = 0;
int tempEchoBack = 0;
volatile int pingNext = frontSonar;

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
float xCoord = 0; 
float yCoord = 0; 
int nextXCoord = 0; 
int nextYCoord = 0; 
int locationsArray[15][2] = {
  0};
int currentArrayRow = 0;
long temporaryLeftCounter; 
long temporaryRightCounter;

// State variables
volatile int mainState = 0; 
int mazeState = 0; 
int seeWallState = 0; 
int lostWallState = 0; 
int cliffState = 0;
int extState = 0; 
int rtnState = 0; 
int armState = 0; 
int drivingDirection = xPos; 
int driveToCandleState = 0;

// boolean variables used in determining when to move to the next state
boolean turnComplete = false; 
boolean disTravComplete = false; 
boolean scanComplete = false;
boolean fanSweepComplete = false;
boolean hitTop = false;
volatile boolean waiting = false;
boolean firstTimeThrough = true;
boolean firstTimeThroughTurning = true;
long tempTimer = 0;
boolean firstTimeThroughWholeProgram = true;
boolean homeIsReached = false;

// Interrupt Variables
volatile long countTime = 0;
volatile long rightCounter = 0;
volatile long leftCounter = 0;

//added by Kevin ----------------------------------------Karen----------------------------you may want to put these elsewhere. I didnt want to screw up the organization
boolean usePID = false; //if 1, the fan arm uses PID to drive. If 0, uses limit switches.
#define upperSwitchPin A5
#define lowerSwitchPin A6
#define drivingUp 0
#define waitingAtTop 1
#define drivingDown 2
int armStateSwitches = drivingUp;
int armRaiseSpeed = 105;
int armHoldSpeed = 100;
int armLowerSpeed = 92;
int armMotorAdjust = 0;
long initUpTime = 0;
long initDownTime = 0;
long armUpTime = 4000;

/*********************************************************************************************/
void setup(){
  
  // setup limit switch pins
  pinMode(upperSwitchPin, INPUT_PULLUP);
  pinMode(lowerSwitchPin, INPUT_PULLUP);
  
  // setup LED array
  pinMode(ledArrayPin,OUTPUT);
  digitalWrite(ledArrayPin,HIGH);

  // setup Fan 
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin,LOW);

  Serial.begin(9600);
  Serial.println("beginSetup");

  // need some way to determine which direction the robot is initially facing 

  // setup drive motors
  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);

  // LCD setup
  lcd.begin(16, 2);

  // setup Cliff sensor
  pinMode(cliffSensorPin, INPUT);

  // setup Fan Motor
  armMotor.attach(armMotorPin);
  pinMode(armPotPin, INPUT);


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
  
  //Ensures all Soonar pin pins are initialized to low
  digitalWrite(frontPingPin,LOW);
  digitalWrite(sidePingPin,LOW);
  digitalWrite(backPingPin,LOW);
  
  //Motor pin declarations
  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);
  
  //Start Button initialization
  pinMode(startButtonPin, INPUT_PULLUP);
  
  // I2C initialization
  Wire.begin();
  Serial.println("gyroStart");

  // Gyro Initializations 
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
  gyro.writeReg(L3G::CTRL_REG4, 0x20); // 2000 dps full scale
  gyro.writeReg(L3G::CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz  

  calcGyroOffset();

  Serial.println("gyroFinish");
  
  //Encoder interrupt initialization
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);
  
  //Sonar interrupt initialization
  attachInterrupt(5,frontSonarISR,CHANGE); 
//  attachInterrupt(4,sideSonarISR,CHANGE);  
  
  // set up interrups for drive Motors
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);  
  
  while(digitalRead(startButtonPin) == HIGH){
    //waiting for button
  }
}

/*********************************************************************************************/

void loop() {
  
  Serial.println();
  //  lcd.setCursor(0,0);
  //  lcd.print(mainState);

  //Serial.println("beginLoop");

  //Serial.println(millis());
//  Serial.print(pingNext);
//  Serial.print("  ");
//  pingNext = frontSonar;
//  Serial.print(getDis(frontSonar));
//  Serial.print("  ");
//  Serial.print(getDis(sideSonar));
//  Serial.print("  ");
//  Serial.print(getDis(backSonar));
//  Serial.print("  ");
//  Serial.print(mazeState);
//  Serial.print("  ");

  ping(pingNext); // continually pings the sonars being used to update their values
  
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
  default:
    Serial.println("HIT MAIN LOOP DEFAULT");
    lcd.print("ERROR 00");
    delay(5000);
    break;
  }
}


/*********************************************************************************************/
// FindFire Switch State

void findFire(void) {

  printPosition();

  // rotates the fire sensor looking for a fire
  lookForFire();
  // checks the cliff detector for a cliff
  if (mazeState != seeingCliff){
    checkForCliff();
  }

//  Serial.print(mazeState);
//  Serial.print("  ");
  
  switch (mazeState) {
  case followingWall:  //
   // Serial.println("followWall");
    followWall();

    if(checkFrontDis(frontWallDist)){
     // Serial.println("seeingWallFront");
      mazeState = seeingWallFront;
    }

    if(checkSideDisGreater(closeWallDist)){ 
     // Serial.println("loosingWall");
      mazeState = loosingWall;
      tempTimer = countTime;
    }

    break;
  case seeingWallFront:
  //  Serial.println("turningBCFront");
    seeWallFront();

    if(turnComplete){
      turnComplete = false;
      mazeState = followingWall;
      seeWallState = seeWallStoppingDrive;
    }

    break;
  case loosingWall:
 //   Serial.println("lostWall");
    lostWall();

    if((lostWallState != lostWallStopping) && (lostWallState != lostWallKeepDrivingStraight)){

      //once sonar can see a wall again
      if(checkSideDisLess(closeWallDist)){

        mazeState = followingWall;
        Kw = wallProportionalVal; 
        lostWallState = 0;
      }
      if(checkFrontDis(frontWallDist)){

        mazeState = seeingWallFront;
        Kw = wallProportionalVal; 
        lostWallState = 0;
      }
    }

    break;
  case seeingCliff: 
    seenCliff();

    if (cliffState == SeenCliffBackOnCourse){
      //once sonar can see a wall again 
      if(checkSideDisLess(closeWallDist)){
        mazeState = followingWall;
        cliffState = 0;
      }
      if(checkFrontDis(frontWallDist)){
        mazeState = seeingWallFront;
        Kw = wallProportionalVal; 
        lostWallState = 0;
      }
    }
    break; 
  default:
    Serial.println("HIT FIND FIRE DEFAULT");
    lcd.println("ERROR 01");
    delay(5000);
  }

}

/*********************************************************************************************/
//Extinguish Fire Switch Statement

void extinguishFire(void){
  printPosition();
  digitalWrite(ledArrayPin,LOW);
  switch (extState) {
  case initialScanning:
    scan();
    stopAllDrive();
    if(scanComplete) {
      extState = drivingToCandle;
      scanComplete = false; 
    }
    break;

  case drivingToCandle: 
    driveToCandle();

    if(checkFrontDis(candleDist)){
      stopAllDrive();
      extState = activatingFan;
      armState = armUp;
    }
    break;
  case activatingFan:
    if(usePID){
      activateFanPID();
    }
    else{
      activateFanSwitches();  
    }

    if(fanSweepComplete){ 
      extState = checkingFlame;
      fanSweepComplete = false;
    }

    break;

  case checkingFlame: 
    scan();

    if(lastFlameVal < 300){ 
    extState = activatingFan; 
    }

    else {
      extState = flameIsOut;
    }
    break; 
  case flameIsOut: 
    reportFlame(); 
    mainState = returningHome;
    break; 
  default:
    Serial.println("HIT EXTINGUISH FIRE DEFAULT");
    lcd.println("ERROR 02");
    delay(5000);

  }
}

/*********************************************************************************************/
// Return Home Switch Statement

void returnHome(void) {
  
  checkForCliff();
  
  digitalWrite(ledArrayPin,HIGH);
  switch (rtnState) {
  case gettingToWallTurning180: 
    turn(pullAUiey);
    if(turnComplete) {
      turnComplete = false; 
      rtnState = gettingBackToWallDrive;
    } 
    break;
  case gettingBackToWallDrive:
    driveStraightForwardEnc();
    if(checkSideDisLess(closeWallDist)){
      stopAllDrive();
      rtnState = straighteningRobot;
      tempTimer = countTime;
    }
    if(checkFrontDis(frontWallDist)){
      
        stopAllDrive();
        rtnState = turningOnce;
      }
    break;  
  case straighteningRobot:
    followWall();
    if((countTime - tempTimer) >= 10){
      stopAllDrive();
      rtnState = gettingCoordinates;
      returnHomeInitDirection(); //changes the driving direction the initial time when the robot is back to the wall
    }
    break;
    
  case turningOnce:
  turn(ninetyDeg);
  
  if(turnComplete){
    rtnState = gettingCoordinates;
    returnHomeInitDirection(); //changes the driving direction the initial time when the robot is back to the wall
  }
  case gettingCoordinates:
    getCoordinates();

    if(currentArrayRow != 0){ 
    rtnState = drivingToCoordinate;
    }

    //else if the next coordinat is our home location
    else { 
      rtnState = drivingToHome;
    }
    break;
  case drivingToCoordinate:
    followWall(); 

    if(checkFrontDis(frontWallDist)){
      stopAllDrive();
      rtnState = determiningDriveDirrection;
    }
    if(checkSideDisGreater(closeWallDist)){
      stopAllDrive();
      rtnState = DrivingToTurnAboutWall; 
    }
      
    break;   
  case DrivingToTurnAboutWall: 
    driveStraightDesDis(forwardDisToTurnAboutWall);
    if(disTravComplete) {
      firstTimeThrough = true;
      rtnState = determiningDriveDirrection;
      disTravComplete = false;
    }
    break;
  case determiningDriveDirrection:
    turn(ninetyDeg);

    if(turnComplete){
      turnComplete = false;
      xCoord = nextXCoord;  // we have reached the next coordinates
      yCoord = nextYCoord;  // we have reached the next coordinates
      rtnState = gettingCoordinates;
    }
    break;
  case drivingToHome:
    driveHome();
    if(homeIsReached){
      mainState = madeItHome;
    }
  
  default:
    Serial.println("HIT RETURN HOME DEFAULT");
    lcd.println("ERROR 03");
    delay(5000);  

  }
}

void calcGyroOffset(void) {

  int i;
  long totalGyroReading = 0;

  for(i = 0; i < 1000; i++) {

    gyro.read();
    totalGyroReading += (int)gyro.g.z;

    delay(5);

  }

  //Serial.println("OFFSET");
  offset = (float)totalGyroReading/1000;
  //Serial.println(offset);

}










