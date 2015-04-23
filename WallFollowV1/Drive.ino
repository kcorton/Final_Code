//Contains all functions pertaining to driving the motors
//and running/using the encoders

/* motor and encoder pins */
int leftMotorF = 10;
int leftMotorB = 9;
int leftEncoderB = 8;
int leftEncoderA = 2;

int rightMotorF = 7;
int rightMotorB = 6;
int rightEncoderB = 5;
int rightEncoderA = 3;

/* These keep track of encoder ticks */
volatile long rightCounter = 0;
volatile long leftCounter = 0;

/* Speeds of the wheels in encoder ticks per second */
int baseSpeed = 550; //encoder ticks per second
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
float desiredDist = 4.0;  //the desired distance between the robot and the wall
float distToWall = 0; //the actual distance between the robot and the wall
float wallError = 0;  //the difference between the above two
float Kw = 100;  //propotional multiplier which affects how much the wallError affects the speed
float velocityError = 0; //the difference between the speeds of the wheels
float Kv = 0.5;  //propotional multiplier which affects how much the velocityError affects the speed
volatile float accelTime;  //used as a multiplier to slow the robot's acceleration
//==========================================================================
void initializeMotors(){
  //Motor pin declarations
  pinMode(leftMotorF, OUTPUT);
  pinMode(leftMotorB, OUTPUT);
  pinMode(rightMotorF, OUTPUT);
  pinMode(rightMotorB, OUTPUT);
  
  //Encoder interrupt initialization
  attachInterrupt(0,leftTick,RISING);
  attachInterrupt(1,rightTick,RISING);  
}
//==========================================================================
//returns the number of ticks held by leftCounter
int getLeftTicks(){
  return leftCounter;
}
//==========================================================================
//returns the number of ticks held by rightCounter
int getRightTicks(){
  return rightCounter;
}
//==========================================================================
//returns the valuse of accelTime
int getAccelTime(){
  return accelTime;  
}
//==========================================================================
//increments accelTime
void incrementAccelTime(){
  if (accelTime < 1){
    accelTime += .05;                             
  }  
}
//==========================================================================
//triggers when encoder signal A goes high and either increments or
// decrements leftCounter based on whether signal B is low or high
void leftTick(){
  if (digitalRead(leftEncoderB) == HIGH){
    leftCounter++; 
  }
  else{
    leftCounter--; 
  }
}
//==========================================================================
//triggers when encoder signal A goes high and either increments or
// decrements rightCounter based on whether signal B is low or high
void rightTick(){
  if (digitalRead(rightEncoderB) == HIGH){
    rightCounter--;
  }
  else{
    rightCounter++;
  }
}
//==========================================================================
//determines whether the wheels should spin forwards or backwards based on
// the sign, then maps the speeds, which are originally in encoder ticks
// per second, to the corresponding analog write value
void updateMotors(){
  if(leftSpeed >= 0){  //drive forward
    analogWrite(leftMotorF,mapSpeed(leftSpeed));
    analogWrite(leftMotorB,0);
  }
  else{                //drive backward
    analogWrite(leftMotorF,0);
    analogWrite(leftMotorB,(mapSpeed(leftSpeed)));
  }
  if(rightSpeed >= 0){ //drive forward
    analogWrite(rightMotorF,mapSpeed(rightSpeed));
    analogWrite(rightMotorB,0);
  }
  else{                //drive backward
    analogWrite(rightMotorF,0);
    analogWrite(rightMotorB,(mapSpeed(rightSpeed)));
  }
}
//==========================================================================
//returns an analogWrite value (0 to 255) corresponding to the input speed
//Note: ignores the sign of inSpeed since negatives are already taken care of
//      of in updateMotors()
int mapSpeed(int inSpeed){
  //get the absolute value of inSpeed
  if(inSpeed < 0){
    inSpeed *= -1;  
  }
  //convert to an analogWrite value (0 to 255)
  return ( (float)inSpeed/9.8 );
}
//==========================================================================
//calculates the average velocity of the robot in encoder ticks per second and
// stores the values in a 2x5 array with circular indexing. The left velocity
// is stored in row 0 and the right is stored in row 1.
//assumes the timer triggers every .1 s, resulting in this function storing
// the last .5 seconds of velocity data
void calcVelocity(){
  if(countTime - lastTime > 0){ // true if the timer went off since the last running of calcVelocity()
    lastTime = countTime;
    //save the current number of ticks so they dont change
    // while calculations are being made
    tempLeftTicks = leftCounter;
    tempRightTicks = rightCounter;
    
    //calculate the change in ticks since the last time this function ran
    leftChange = tempLeftTicks - lastLeftTicks;
    rightChange = tempRightTicks - lastRightTicks;
    
    //calculate the velocity (assumes this function runs every .1 seconds
    leftVelocity = leftChange/.1;
    rightVelocity = rightChange/.1;
    
    //set the new initial ticks in preparation for the next run
    lastLeftTicks = tempLeftTicks;
    lastRightTicks = tempRightTicks; 
    
    //increment the storing index
    if(nextIndex >= 4){
      nextIndex = 0;  
    }
    else{
      nextIndex++;
    }
    
    //store the left and right velocities in a 2x5 array
    speedStorage[0][nextIndex] = leftVelocity;
    speedStorage[1][nextIndex] = rightVelocity; 
  }
}
//==========================================================================
//calculates the average velocity of the left wheel in encoder ticks per second
// using the data stored in speedStorage[0]
float getLeftVeloc(){
  int sum = 0;
  //sum the stored velocities
  for(int i = 0; i < 5; i++){
    sum += speedStorage[0][i];  
  }
  //calculate and return the average
  return sum/5;
}
//==========================================================================
//calculates the average velocity of the right wheel in encoder ticks per second
// using the data stored in speedStorage[1]
float getRightVeloc(){
  int sum = 0;
  //sum the stored velocities
  for(int i = 0; i < 5; i++){
    sum += speedStorage[1][i];  
  }
  //calculate and return the average
  return sum/5;
}
//==========================================================================
//follows a wall at a distance of desiredDist at an average speed
// set by baseSpeed
void followWall(){
  calcVelocity();
  //calculate sonar error
  distToWall = getDis(SIDESONAR);
  wallError = desiredDist - distToWall;
  //calculate velocity error
  velocityError = getRightVeloc() - getLeftVeloc();
  Serial.print(leftSpeed);
  Serial.print("  ");
  Serial.print(rightSpeed);
  Serial.print("  ");
//  Serial.print(getLeftVeloc());
//  Serial.print("  ");
//  Serial.println(getRightVeloc());
  //calculate speed based on the errors (proportional control)
  leftSpeed = accelTime * (baseSpeed + (Kw * wallError) + (Kv * velocityError));
  rightSpeed = accelTime * (baseSpeed - (Kw * wallError) - (Kv * velocityError));
  //set the motor speeds
  updateMotors();
}
