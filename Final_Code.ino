// Final Code Goes Here 

/*********************************************************************************************/
/* Necessary Include Files */


#define findingFire 0
#define extinguishingFire 1
#define returningHome 2

#define scaning 0
#define turningToFlame 1
#define drivingToCandle 2
#define activatingFan 3
#define checkingFlame 4
#define flameIsOut 5

#define candleDist 1



int mainState = 0; 
int extState = 0; 


void setup(){


}

void loop() {
  switch (mainState) {
  case findingFire:
    findFire();
    break;
  case extinguishingFire:
    extinguishFire();
    break;
  case returningHome:
    returnHome();
    break;
  }
}


/*********************************************************************************************/
// FindFire Switch State

void findFire(void) {

}

/*********************************************************************************************/
//Extinguish Fire Switch Statement

void extinguishFire (void){
  switch (extState) {
  case scaning: // scans full range of fire sensor
    scan();
    
    // if scan is complete 
    extState = turningToFlame;
    
    break;
    
  case turningToFlame: // uses highest fire value's servo position and gyro to rotate robot towards flame
    turnTowardFlame();
    
    //if turn is complete
    extState = drivingToCandle; 
    
    break;
    
  case drivingToCandle: 
    driveToCandle();
    
      if(checkFrontDis(candleDist)){
    extState = activatingFan ;
  }
    break;
  case activatingFan:
    activateFan();
    
      //If fan sweep is complete 
  extState = checkingFlame;
  
    break;
    
  case checkingFlame: 
    scan();
    
    // if scan returns interrupt on Fire Sensor 
    extState = activatingFan; 
    
    // else 
    
    extState = flameIsOut ; 
   case flameIsOut: 
     reportFlame(); 
     mainState = returningHome;
    break; 


  }
}

/*********************************************************************************************/
// Return Home Switch Statement

void returnHome (void) {

}

/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a variable with the servo position at the highest flame sensing

void scan (void) {
  

}

/*********************************************************************************************/
// Turn Toward Flame function
// turns robot to the angle specified based on the variable changed in the scan function

void turnTowardFlame(void){

}

/*********************************************************************************************/
// Drive to Candle Function

void driveToCandle (void) {


  scan(); 
  turnTowardFlame();
  driveStraightForward();

}

/*********************************************************************************************/
// Drive Straight Frward function

//drives straight by ensuring both encoders have moved the same distance

void driveStraightForward(void) {

}

/*********************************************************************************************/
// Check Front Distance function 

// if the front distance sensors sees something  a certain distance away the function is true

boolean checkFrontDis(int desDis){

  if (getFrontDis() <= desDis) { 
    return true; 
  }

  else { 
    return false; 
  }

}

/*********************************************************************************************/
// Get Front Distance Function  

// returns the value from the front distance sensor when called 

int getFrontDis(void) {

}

/*********************************************************************************************/
// Activate Fan Function 

// Sweeps Fan between it's two extreme postions and checks if the fire is still detected 

void activateFan (void) { 
  
  // run fan between two extremes 
  
  
}

/*********************************************************************************************/
// Report Flame Function 

// reports the location of the flame 

void reportFlame(void) {
  
}
  
  

