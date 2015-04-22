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
int leftSpeed = 100;
float rightDist = 0;
float rightDistTemp =0; 
int rightSpeed = 100;

volatile int accelTime;

int desiredDist = 30;

float distCenter = 0;
float distFront = 0;

float inchesPerTick = .0032751103;

float velocityLeft = 0.0;
float velocityRight = 0.0;
boolean countTriggered = false;
long lastLeft = 0;
long lastRight = 0;

int fanPin = 22;
//================================================================================================================
void setup(){

  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin,LOW);
  
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
void calcVelocity(){
  if(countTriggered){
    velocityLeft = (leftCounter - lastLeft) / 0.1;
    velocityRight = (rightCounter - lastRight) / 0.1;
    countTriggered = false;
    Serial.print(velocityLeft);
    Serial.print("  ");
    Serial.println(velocityRight);
  }
}
//================================================================================================================
void timerISR(void) {
  // we are here because the timer expired and generated an interrupt
  if (accelTime <= 10){
    accelTime++;                             
  }
  countTriggered = true;
} 
//================================================================================================================
void loop(){
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
}
//================================================================================================================
float ticksToInches(long int ticks){ 
  return (inchesPerTick * ticks);
}
//================================================================================================================
void leftTick(){
  if (digitalRead(leftEncoderB) == HIGH){
    leftCounter++; 
  }
  else{
    leftCounter--;
  }
}
//================================================================================================================
void rightTick(){
  if (digitalRead(rightEncoderB) == HIGH){
    rightCounter++;
  }
  else{
    rightCounter--;
  }
}
