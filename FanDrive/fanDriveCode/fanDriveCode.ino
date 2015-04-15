/*Charlie wrote this code. It runs the fan, and at the same time, raises the fan arm, then lowers it.
once the fan is lowered, the fan should continue spinning. To stop this, add analogWrite(fanMotor, 0); in the 
change state, along with the boolean hitTop = false; This will essentially reset all the variables for this state,
which would be good for using a state machine.*/

#include<Servo.h>

int fanMotor = 22;
Servo armMotor;
int armPot = A4;
int highPos = 158;
int lowPos = 115;
boolean hitTop = false; //need to use this line within the change state, to reset to false.

//==========================================================================================
void setup(){
  armMotor.attach(4);
  pinMode(armPot, INPUT);
  Serial.begin(9600);
}

//==========================================================================================

void loop(){
  analogWrite(fanMotor, 255);
  if(!hitTop){
    armMotor.write(70);
    if(analogRead(armPot) > highPos){
      hitTop = true;
    }
  }
  else if(analogRead(armPot) > lowPos){
    armMotor.write(110);
  }
  else{
    armMotor.write(90);
  }
  Serial.println(hitTop);
}

  
