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
#define upperSwitchPin A5
#define lowerSwitchPin A6

// Main State machine States
#define findingFire 0
#define extinguishingFire 1
#define returningHome 2
#define madeItHome 3

//Finiding Fire State Machine States
#define followingWall 0
#define seeingWallFront 1
#define losingWall 2
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
#define backUp 0
#define gettingToWallTurning180 1
#define gettingBackToWallDrive 2
#define straighteningRobot 3
#define turningOnce 4
#define gettingCoordinates 5
#define drivingToCoordinate 6
#define DrivingToTurnAboutWall 7
#define determiningDriveDirrection 8
#define determiningDriveDirrection2 9
#define drivingToHome 10
#define seeingCliffReturning 11

//Fan Sweep State Machine
#define drivingUp 0
#define waitingAtTop 1
#define drivingDown 2

//Turning State Machine 
#define xPos 0
#define yPos 1
#define xNeg 2
#define yNeg 3

// Sonar Names 
#define frontSonar 0
#define sideSonar 1
#define backSonar 2 // this is the side sonar used when driving back to home

// Array Columns for storing points as the maze is traversed
#define xCol 0
#define yCol 1

// Pre-defined Values and Distances
#define candleDist 4 // the distance away from the candle when the robot will stop
#define frontWallDist 5  // the distance from the front wall when the robot should stop
#define desiredDist 4  //the desired distance between the robot and the side wall
#define closeWallDist 6 // if a wall is within this value there is a close wall to be followed
#define backFromFlame -250 // how far the robot should back up from the candle to return home in encoder ticks
#define negBackFromCliffDist -500  //(5 inches) how far the robot will back up after seeing a cliff in encoder ticks
#define forwardDisToTurnAboutWall2 5000  // how far the robot drives forward after loosing the wall driving forward and turning to turn about the penensula 
#define forwardDisToTurnAboutWall 1500//(2 inches) how far the robot drives straight once it's lost the wall in encoder ticks
#define ninetyDeg 90 //ninety degrees used for turning
#define negNinetyDeg -90  // negative ninety degrees used for turning
#define pullAUiey 180 // 180 degrees used for turning 
#define inchesPerTick .0066403062 // the number of inches driven per encoder tick
#define turningSpeed 400 // the speed the robot turns at
#define wallProportionalVal 100 //the variable used as proportional control with the distance from the wall
#define velocityProportionalVal 0.5 // the variable used as a proportional control with the velocities of the wheels

//LCD setup
LiquidCrystal lcd(13, 12, 17, 16, 15, 14);

// Motor and Servo Declarations
Servo armMotor;
Servo flameServo;

//Variable for Driving To Candle
long initTimeCandleDrive = 0; // time variable used to drive forward towards the candle for a certain time
int candleTurn = 0; //the degrees the robot actually turns to face the candle

// Variables for Fire Sensor Functions
int servoIncreasing = 0; // determines if the servo is increasing or decreasing
int fireServoPos = 0; //the position of the servo with the fire sensor attached
long lastFireTimeCount = 0; // time value used to run the finding fire function if a given amount of time has passed
int firePosition; // the position of the servo where the fire is 
int lastFlameVal = 2000; // the most extreme value from the fire sensor  
int flameAngle = 0; // the angle the robot is driving relative to positive x when driving to candle

//Varables for Driving Functions
int baseSpeed = 700; //encoder ticks per second
int leftSpeed = baseSpeed;  // sets the left speed to the base speed
int rightSpeed = baseSpeed; // sets the right speed to the base speed

//   Variables used by calcVelocity()   
float speedStorage[2][5]; //stores five samples of the velocity from the last .5 seconds
int lastLeftTicks = 0; //holds the value of leftCounter from the previous run of calcVelocity() 
int lastRightTicks = 0; //holds the value of rightCounter from the previous run of calcVelocity()
int tempLeftTicks = 0; //holds the captured value of leftCounter
int tempRightTicks = 0; //holds the captured value of rightCounter
int leftChange = 0; // change in ticks since the last run
int rightChange = 0; // change in ticks since the last run
float leftVelocity = 0; //velocity of left motor in encoder ticks per second
float rightVelocity = 0; //velocity of right motor in encoder ticks per second
int nextIndex = 0; //the index currently being saved to in speedStorage
long lastTime = 0; //holds the time of the last running of calcVelocity()

/* Variables used by followWall() */
float distToWall = 0; //the actual distance between the robot and the wall
float wallError = 0;  //the difference between the actual disance to the wall and the desired value
float Kw = wallProportionalVal;  //propotional multiplier which affects how much the wallError affects the speed
float velocityError = 0; //the difference between the speeds of the wheels
float Kv = velocityProportionalVal;  //propotional multiplier which affects how much the velocityError affects the speed

//variables used in Fan Sweep Function
int armRaiseSpeed = 110;
int armHoldSpeed = 100;
int armLowerSpeed = 92;
int armMotorAdjust = 0;
long initUpTime = 0;
long initDownTime = 0;
long armUpTime = 4000;

//Variables for using the Sonar 
volatile long frontPingTime = 0;  // the time when the last front ping was sent 
volatile long sidePingTime = 0; // the time when the last side ping was sent 
long backPingTime = 0; // the time when the last back ping was sent 
volatile long frontEchoTime = 0; // the time it took from sending the ping to get the ping back exculding bad data
volatile long sideEchoTime = 0; // the time it took from sending the ping to get the ping back exculding bad data
volatile long backEchoTime = 0; // the time it took from sending the ping to get the ping back exculding bad data
volatile int tempEchoFront = 0; // the actual time it took from sending the ping to get the ping back not exculding bad data
int tempEchoSide = 0; // the actual time it took from sending the ping to get the ping back not exculding bad data
int tempEchoBack = 0; // the actual time it took from sending the ping to get the ping back not exculding bad data
volatile int pingNext = frontSonar; // variable used to determine which sonar to ping next

// variables used for turning with the gyro
L3G gyro; // initializes the gyro
int currGyroReading; // the reading from the gyro
int mdps = 0;  // milli degrees per second the robot is turning
float totalDegrees = 0; // total degrees the robot has turned
long lastTurnCount = 0; // time variable used to check the gyro every tenth of a second
long lastTurnTime = 0;  // the time the turn started
long currTurnTime = 0; // the length the turn has been occuring for
int offset = 0; // the offset of the gyro calculated in setup

// Variables to keep track of where the robot is and has been
float xCoord = 0; // current xCoordinate of the robot
float yCoord = 0; // current yCoordinate of the robot
float nextXCoord = 0; // when returning home the next x coordinate to be reached
float nextYCoord = 0; // when returning home the next y coordinat to be reached
int locationsArray[30][2] = {0}; //an array of all the places the robot has made 90 degree turns
int currentArrayRow = 0; // the current array row being accessed
long temporaryLeftCounter; //a temporary encoder count value used to determine how far the robot drives
long temporaryRightCounter; //a temporary encoder count value used to determine how far the robot drives
long averageDistance = 0; 

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
int armStateSwitches = 0;

// boolean variables used in determining when to move to the next state
boolean turnComplete = false; // set to true once a turn is complete and then set back to false to be used in the next turn
boolean disTravComplete = false; // set to true once the desired distance has been traveled
boolean scanComplete = false; // set to true once the scan for fire is complete
boolean fanSweepComplete = false; // set tu true when the fan sweep is complete
boolean hitTop = false; // set to true when the top limit switch is hit with the fan
volatile boolean waiting = false; // set to true when the sonar is waiting to recieve a ping back
boolean firstTimeThrough = true; // set to false if it is not the first time calling a function, used to set or change variables once and not every time a function is called
boolean firstTimeThroughTurning = true; // set to false if it is not the first time through the turning function
long tempTimer = 0; // a temporary timer used throughout 
boolean homeIsReached = false; // set to true when the robot gets home
boolean fireStillExists = false; // set to true when the fire still exists
boolean robotIsTurning = false; // set to true when the robot is turning

// Interrupt Variables
volatile long countTime = 0; // the total time the program has been runing in .1s
volatile long rightCounter = 0; // the count of encoder ticks from the right motor
volatile long leftCounter = 0; // the count of encoder ticks from the left motor
volatile float accelTime;  //used as a multiplier to slow the robot's acceleration as it starts to drive

/*********************************************************************************************/
void setup(){
  
  // serial monitor setup
  Serial.begin(9600);
  Serial.println("beginSetup");

  // setup limit switch pins
  pinMode(upperSwitchPin, INPUT_PULLUP);
  pinMode(lowerSwitchPin, INPUT_PULLUP);

  // setup LED array
  pinMode(ledArrayPin,OUTPUT);
  digitalWrite(ledArrayPin,HIGH);

  // setup Fan 
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin,LOW);

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
  
  // sets up timer used throughout functions
  Timer1.initialize(100000); // interrupt every .1s or 10 times every second
  Timer1.attachInterrupt(timerISR);

  //Encoder interrupt initialization
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);

  //Sonar interrupt initialization
  attachInterrupt(5,frontSonarISR,CHANGE);   

  // set up interrups for drive Motors
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING); 
 
 
// waits for button press to begin driving
  while(digitalRead(startButtonPin) == HIGH){
    //waiting for button
  }
}

/*********************************************************************************************/

void loop() {

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

// the robot here is navigating the maze looking for the fire

void findFire(void) {

  printPosition();  // prints the position of the robot to the LCD screen

  // rotates the fire sensor looking for a fire
  lookForFire();
  
  // checks the cliff detector for a cliff if a cliff has not already been seen
  if (mazeState != seeingCliff){
    checkForCliff();
  }

  switch (mazeState) {
  case followingWall:  // following a wall using Sonar and Velocities of the motors to drive straight
    followWall();

    // if a wall is seen with the front sonar change to that state
    if(checkFrontDis(frontWallDist)){
      // Serial.println("seeingWallFront");
      mazeState = seeingWallFront;
    }

    // If the side wall has been lost change to that state
    if(checkSideDisGreater(closeWallDist)){ 
      // Serial.println("losingWall");
      mazeState = losingWall;
      tempTimer = countTime;
    }

    break;
    
  case seeingWallFront:
    
    // this state machine stops the robot and turn it 90 degrees
    seeWallFront();

    // once the turn is complete go back to following the wall
    if(turnComplete){
      turnComplete = false;
      mazeState = followingWall;
      seeWallState = seeWallStoppingDrive;
    }

    break;
    
  case losingWall:
    // this state machine navigates the robot about a peninsula 
    lostWall();

    // if it is not in the first two states check the sonar, otherwise ignore the sonar readings as they may pick up the edge of the wall and get bad data
    if((lostWallState != lostWallStopping) && (lostWallState != lostWallKeepDrivingStraight)){

      //once sonar can see a wall again to the side follow it
      if(checkSideDisLess(closeWallDist)){

        mazeState = followingWall;
        Kw = wallProportionalVal; 
        lostWallState = 0;
      }
      // if the sonar in the front can see a wall go to the state where a front wall is seen
      if(checkFrontDis(frontWallDist)){
        mazeState = seeingWallFront;
        Kw = wallProportionalVal; 
        lostWallState = 0;
      }
    }

    break;
    
  case seeingCliff: 
    // this state is entered from an interrupt if a cliff is seen
    seenCliff();

    // once the robot is back driving straight check the sonar again
    if (cliffState == SeenCliffBackOnCourse){
      //once sonar can see a wall again to the side follow it
      if(checkSideDisLess(closeWallDist)){
        mazeState = followingWall;
        cliffState = 0;
      }
      // if the sonar on the front can see a wall, turn about it
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
  
  printPosition();  // prints the position of the robot
  digitalWrite(ledArrayPin,LOW);  // changes the LED flashing sequence
  switch (extState) {
  case initialScanning:
    // scans the servo range looking for the highest fire value
    scan();
    stopAllDrive();
    // once the scan is complete drive towards the candle
    if(scanComplete) {
      extState = drivingToCandle;
      scanComplete = false; 
    }
    break;

  case drivingToCandle: 
    // scans, turns, and drives the robot towards the fire repeatadely 
    driveToCandle();
  
    //if its not scanning or turning check the front sonar for the candle
    if((driveToCandleState != scanning) && (driveToCandleState != turningToCandle)){

      // once the candle base is seen
      if(checkFrontDis(candleDist)){
        stopAllDrive();
        extState = activatingFan;
      }
    }
    break;
    
  case activatingFan:
      // activates the fan and runs it from top to bottom
      activateFanSwitches();

    if(fanSweepComplete){ 
      extState = checkingFlame;
      fanSweepComplete = false;
    }

    break;

  case checkingFlame: 
    // scans to see if the fire is still going
    scan();
    if(scanComplete){
      scanComplete = false;
      
      // if the fire still exists go back to finding it
      if(fireStillExists) { 
        extState = drivingToCandle;
      }

      else {
        extState = flameIsOut;
      }
    }
    break; 
  case flameIsOut: 
    // calculate the final candle location and return home
    updateAngleDriveLocation();
    adjustFlamePos();
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

  // continue checking for a cliff
  checkForCliff();

  digitalWrite(ledArrayPin,HIGH);  // change the LED's because the fire is out
  switch (rtnState) {
  case backUp:
    // backs up away from the candle
    driveStraightDesDis(backFromFlame);
    if(disTravComplete){
      disTravComplete = false;
      rtnState = gettingToWallTurning180;
      leftSpeed = baseSpeed;
      rightSpeed = baseSpeed;
    }
    break;
  case gettingToWallTurning180: 
    // turn 180 degrees to drive home
    turn(pullAUiey);
    if(turnComplete) {
      turnComplete = false; 
      rtnState = gettingBackToWallDrive;
    } 
    break;
  case gettingBackToWallDrive:
    // drive straight 
    driveStraightForwardEnc();
    
    // if you can see a wall to the side straighten the robot to the wall
    if(checkSideDisLess(closeWallDist)){
      stopAllDrive();
      rtnState = straighteningRobot;
      tempTimer = countTime;
    }
    
    // if you see a wall to the front turn once
    else if(checkFrontDis(frontWallDist)){ 
      stopAllDrive();
      rtnState = turningOnce;
    }
    break;  
    
  case straighteningRobot:
  // follow the wall for a second to straighten the robot to the wall
    followWall();
    if((countTime - tempTimer) >= 10){
      stopAllDrive();
      rtnState = gettingCoordinates;
      returnHomeInitDirection(); //changes the driving direction the initial time when the robot is back to the wall
    }
    break;

  case turningOnce:
  // turn the robot 90 degrees to follow the wall
    turn(ninetyDeg);

    if(turnComplete){
      turnComplete = false;
      rtnState = gettingCoordinates;
      returnHomeInitDirection(); //changes the driving direction the initial time when the robot is back to the wall
    }
    break;
  case gettingCoordinates:
  // gets the next coordinates to drive to
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
  // follow the wall until you need to turn
    followWall(); 
    
    // if there is a wall to the front turn around it
    if(checkFrontDis(frontWallDist)){
      stopAllDrive();
      rtnState = determiningDriveDirrection;
    }
    
    // if the wall to the side is lost turn about it
    if(checkSideDisGreater(closeWallDist)){
      stopAllDrive();
      rtnState = DrivingToTurnAboutWall; 
    }

    break;   
  case DrivingToTurnAboutWall: 
    //drive forward to turn about the wall
    driveStraightDesDis(forwardDisToTurnAboutWall);
    if(disTravComplete) {
      firstTimeThrough = true;
      rtnState = determiningDriveDirrection2;
      disTravComplete = false;
    }
    break;
    
  case determiningDriveDirrection:
  
    // turn ninety degrees
    turn(ninetyDeg);

    if(turnComplete){
      turnComplete = false;
      xCoord = nextXCoord;  // we have reached the next coordinates update our location
      yCoord = nextYCoord;  // we have reached the next coordinates update our location 
      rtnState = gettingCoordinates;
    }
    break;

  case determiningDriveDirrection2:
  
    // turn negative ninety Degrees
    turn(negNinetyDeg);

    if(turnComplete){
      turnComplete = false;
      xCoord = nextXCoord;  // we have reached the next coordinates update our location 
      yCoord = nextYCoord;  // we have reached the next coordinates update our location 
      rtnState = gettingCoordinates;
    }
    break;
    
  case drivingToHome:
  
    // drives the correct distance based on the difference in the coordinates to get home 
    driveHome();
    if(homeIsReached){
      mainState = madeItHome;
    }
    break;
  case seeingCliffReturning:
  
    // this is entered by an interrupt if a cliff is seen 
    seenCliff();  
    break;  
  default:
    Serial.println("HIT RETURN HOME DEFAULT");
    lcd.println("ERROR 03");
    delay(5000);  

  }
}

/*********************************************************************************************/
// Called in the setup to calcualte the gyro offset

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












