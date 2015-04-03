//#include <ultrasonic.h>
//call the ultrasonic constructor
//ultrasonic(INPUTPIN, OUTPUTPIN)
//ultrasonic ultrasonicC(23, 24);
//ultrasonic ultrasonicF(25, 26);

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

float leftDist = 0;
int leftSpeed = 0;
float rightDist = 0;
int rightSpeed = 0;

int desiredDist = 5;

float distCenter = 0;
float distFront = 0;

//1/32 rev/tick
//50:1 internal ratio
//5:3 external ratio
//2.78in diameter wheel --> 2.78*pi in/rev
//
float inchesPerTick = .0032751103;


void setup(){
  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);
  
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);
  
  Serial.begin(9600);
}

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
  
  //distCenter = ultrasonicC.distance();
  //distFront = ultrasonicF.distance();
  
  //Serial.print(distCenter, DEC);
  //Serial.print("     ");
  //Serial.println(distFront, DEC);
  
  //if (dist > 108.0) dist = 108;
  
  Serial.print(leftCounter);
  Serial.print(" ");
  Serial.println(rightCounter);
  Serial.print(leftDist);
  Serial.print(" ");
  Serial.println(rightDist);
  Serial.println();
  
  leftDist = ticksToInches(leftCounter);
  rightDist = ticksToInches(rightCounter);
  
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

