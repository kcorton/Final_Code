#include <TimerOne.h>

int leftMotorF = 10;
int leftMotorB = 9;
int leftEncoderB = 8;
int leftEncoderA = 2;
int rightMotorF = 7;
int rightMotorB = 6;
int rightEncoderB = 5;
int rightEncoderA = 3;

float distToGoL = 0.0;
float distToGoR = 0.0;
int baseSpeed = 100;
int travelSpeed = 100;

float Ka = 1;
int Kb = 1;

int state = 0; 

volatile long int rightCounter = 0;
volatile long int leftCounter = 0;
volatile long int rightCounterTemp = 0; 
volatile long int leftCounterTemp = 0; 

float leftDist = 0;
float leftDistTemp =0; 
int leftSpeed = 80;
float rightDist = 0;
float rightDistTemp =0; 
int rightSpeed = 80;

volatile int accelTime;

int desiredDist = 30;

float distCenter = 0;
float distFront = 0;

float inchesPerTick = .0032751103;
//================================================================================================================
void setup(){

  /* set up the timer */
  Timer1.initialize(100000);	               
  Timer1.attachInterrupt(timerISR);           

  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);

  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);

  Serial.begin(9600);
}
//================================================================================================================
void timerISR(void) {
  // we are here because the timer expired and generated an interrupt
  if (accelTime <= 10){
    accelTime++;                             
  }
} 
//================================================================================================================
void loop(){

  leftDist = ticksToInches(leftCounter);
  rightDist = ticksToInches(rightCounter);

  leftDistTemp = ticksToInches(leftCounterTemp);
  rightDistTemp = ticksToInches(rightCounterTemp);

  if(leftSpeed >= 0){
    analogWrite(leftMotorF,leftSpeed);
    analogWrite(leftMotorB,0);
  }
  else{
    analogWrite(leftMotorF,0);
    analogWrite(leftMotorB,leftSpeed);
  }
  if(rightSpeed >= 0){
    analogWrite(rightMotorF,rightSpeed);
    analogWrite(rightMotorB,0);
  }
  else{
    analogWrite(rightMotorF,0);
    analogWrite(rightMotorB,rightSpeed);
  }
  
  Serial.print(leftCounter);
  Serial.print("  ");
  Serial.println(rightCounter);

  //proportionalDrive(leftCounter,rightCounter,leftDist,rightDist);
}
//================================================================================================================
void proportionalDrive(long int leftTicks, long int rightTicks, float distL, float distR) {
  //acceleration
  if(accelTime <= 10){
    baseSpeed = accelTime;
  }
  else{
   baseSpeed = travelSpeed; 
  }
  
  distToGoL = desiredDist - distL;
  distToGoR = desiredDist - distR; 
  
  //                keeps robot straight                        slows/stops robot at desired distance
  leftSpeed = (baseSpeed - Ka * (leftTicks - rightTicks)) * (distToGoL/sqrt((distToGoL * distToGoL) + Kb));
  //                keeps robot straight                        slows/stops robot at desired distance
  rightSpeed = (baseSpeed + Ka * (leftTicks - rightTicks)) * (distToGoR/sqrt((distToGoR * distToGoR) + Kb));
  
  Serial.print(leftCounter);
  Serial.print("  ");
  Serial.println(rightCounter);
}

//================================================================================================================
float ticksToInches(long int ticks){ 
  return (inchesPerTick * ticks);
}
//================================================================================================================
void leftTick(){
  if (digitalRead(leftEncoderB) == HIGH){
    leftCounter++; 
    leftCounterTemp--;
  }
  else{
    leftCounter--;
    leftCounterTemp++; 
  }
}
//================================================================================================================
void rightTick(){
  if (digitalRead(rightEncoderB) == HIGH){
    rightCounter--;
    rightCounterTemp++; 
  }
  else{
    rightCounter++;
    rightCounterTemp--; 
  }
}
