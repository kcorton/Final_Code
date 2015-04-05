#include <TimerOne.h>

int leftMotorF = 12;
int leftMotorB = 11;
int leftEncoderB = 10;
int leftEncoderA = 2;
int rightMotorF = 9;
int rightMotorB = 8;
int rightEncoderB = 7;
int rightEncoderA = 3;

int state = 0; 

volatile long int rightCounter = 0;
volatile long int leftCounter = 0;
volatile long int rightCounterTemp = 0; 
volatile long int leftCounterTemp = 0; 

float leftDist = 0;
float leftDistTemp =0; 
int leftSpeed = 0;
float rightDist = 0;
float rightDistTemp =0; 
int rightSpeed = 0;

int desiredDist = 5;

float distCenter = 0;
float distFront = 0;

float inchesPerTick = .0032751103;

void setup(){

  /* set up the timer */
  Timer1.initialize(1000);	               // set up a 1 millisecond timer period
  Timer1.attachInterrupt(timerISR);           // ...and specify the timer ISR

  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);

  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);

  Serial.begin(9600);
}

void timerISR(void) {
  // we are here because the timer expired and generated an interrupt
  state = 1;                                
} 

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

  switch (state) {
  case 0: 
    checkTotalDistance();
    state = 0; 
  case 1: 
    equalizeDrive();
    state = 0; 

  }
}

void checkTotalDistance(void){

  if(leftDist >= desiredDist){
    leftSpeed = 0;
  }
  else{
    leftSpeed = 100;
  }

  if(rightDist >= desiredDist){
    rightSpeed = 0;
  }
  else{
    rightSpeed = 100;
  }

}

void equalizeDrive() {

  if(leftDistTemp < rightDistTemp){
    leftSpeed = 0; 
    rightSpeed = 100;
  }

  else if (rightDistTemp > leftDistTemp){
    leftSpeed = 100;
    rightSpeed = 0; 

  }

  else {
    state = 0; 
    leftCounterTemp = 0; 
    rightCounterTemp =0;
  }
}


float ticksToInches(long int ticks){ 
  return (inchesPerTick * ticks);
}

void leftTick(){
  if (digitalRead(leftEncoderB) == HIGH){
    leftCounter++; 
    leftCounterTemp ++;
  }
  else{
    leftCounter--;
    leftCounterTemp --; 
  }
}

void rightTick(){
  if (digitalRead(rightEncoderB) == HIGH){
    rightCounter--;
    rightCounterTemp --; 
  }
  else{
    rightCounter++;
    rightCounterTemp --; 
  }
}




