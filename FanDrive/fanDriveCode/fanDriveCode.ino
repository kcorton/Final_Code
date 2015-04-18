#include <TimerOne.h>

/*Charlie wrote this code. It runs the fan, and at the same time, raises the fan arm, then lowers it.
once the fan is lowered, the fan should continue spinning. To stop this, add analogWrite(fanMotor, 0); in the 
change state, along with the boolean hitTop = false; This will essentially reset all the variables for this state,
which would be good for using a state machine.*/

#include<Servo.h>

//===========================Pin Attachments===============================================

int fanMotor = 22;
Servo armMotor;
int armMotorPin = 4;
int armPot = A4;

//=============================Variables===================================================

int armState = 0;     // most of these need to be put in the switch to this state statement
int armTimePassed, armInitTime;
int armWaitTime = 100;
int highPos = 460;
int lowPos = 320;
int countTime = 0;
boolean hitTop = false;

//==========================================================================================
void setup(){
  Timer1.initialize(100000); // interrupt every .1s or 10 times every second
  Timer1.attachInterrupt(timerISR);
  armMotor.attach(armMotorPin);
  pinMode(armPot, INPUT);
  armInitTime = countTime;
  Serial.begin(9600);
  
}

//==========================================================================================

void loop(){
  Serial.println(analogRead(armPot));
  armTimePassed = countTime - armInitTime;
  switch(armState){
    case 0:
    if(analogRead(armPot) > highPos){
      armMotor.write(100);
    }
    else{
      armMotor.write(108);
    }
    if(armTimePassed > armWaitTime){
      armState = 1;
    }
    break;
    
    case 1:
    armMotor.write(93);
    break;
  }
}

void timerISR(){
  countTime++;
}

  
