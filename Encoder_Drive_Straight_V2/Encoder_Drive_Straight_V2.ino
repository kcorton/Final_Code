#include <TimerOne.h>

int leftMotorF = 12;
int leftMotorB = 11;
int leftEncoderB = 10;
int leftEncoderA = 2;
int rightMotorF = 9;
int rightMotorB = 8;
int rightEncoderB = 7;
int rightEncoderA = 3;


volatile long int rightCounter = 0;
volatile long int leftCounter = 0;
volatile long int rightCounterTemp = 0; 
volatile long int leftCounterTemp = 0; 

float leftDist = 0;
int leftSpeed = 0;
float rightDist = 0;
int rightSpeed = 0;
float diffOfDis = 0; 

volatile int timerCount = 10; 

int baseSpeed = 100;

int desiredDist = 20;

int kp = 10;

float distCenter = 0;
float distFront = 0;

float inchesPerTick = .0032751103;
//float inchesPerTick = .0016375552;

void setup(){

//  /* set up the timer */
//  Timer1.initialize(100000);	               // set up a 1 tenth on a second timer period
//  Timer1.attachInterrupt(timerISR);           // ...and specify the timer ISR

  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);

  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);

  Serial.begin(9600);
}


void loop(){

  leftDist = ticksToInches(leftCounter);
  rightDist = ticksToInches(rightCounter);

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

  equalizeDrive();
  
  Serial.println("Left Speed, Right Speed");
  Serial.println(leftSpeed);
  Serial.println(rightSpeed);


}

void timerISR(void) {
  //we are here because the timer expired and generated an interrupt
  timerCount++;                                
} 

void equalizeDrive() {

  diffOfDis = leftDist-rightDist;

  if(timerCount <= 10){

    baseSpeed = 10 * timerCount;

  }

  if(leftDist >= desiredDist){
    leftSpeed = 0;
  }
  else{
    leftSpeed = baseSpeed - (kp * diffOfDis);
  }

  if(rightDist >= desiredDist){
    rightSpeed = 0;
  }
  else{
    rightSpeed = baseSpeed + kp * diffOfDis;
  }

}


float ticksToInches(long int ticks){ 
  return (inchesPerTick * ticks);
}

void leftTick(){
  if (digitalRead(leftEncoderB) == HIGH){
    leftCounter++; 
  }
  else{
    leftCounter--;
  }
}

void rightTick(){
  if (digitalRead(rightEncoderB) == HIGH){
    rightCounter--;
  }
  else{
    rightCounter++; 
  }
}






