#include <Servo.h> 
#include <TimerOne.h>
 
Servo flameServo;

#define firePin A1
#define extinguishingFire 1
int mainState = 0;
int servoIncreasing = 0;
int fireServoPos = 0;
volatile long countTime = 0;
int scanComplete = false;
long lastFireTimeCount = 0;

int firePosition; 
int lastFlameVal = 2000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  flameServo.attach(11, 544, 2400);
  
  Timer1.initialize(10000); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}

void loop() {

  switch(mainState) {
  
  case 0: 
    lookForFire();
    break;
  case 1:
    scan();
    if(scanComplete) {
      scanComplete = false;
      delay(5000);
    }
    break;
    
  }
}

void timerIsr(void) {
  
  countTime++;
  
}


/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a global variable with the servo position at the highest flame sensing
// this is used once the flame has been seen

void scan(void) {
  
  int currFlameVal;
  
  if(countTime - lastFireTimeCount > 28) {
  
    if(fireServoPos > 180) {
      
      fireServoPos = 0; // reset the position
      lastFlameVal = 2000; // reset the last flame val
      flameServo.write(0); // write the servo back to reset position
      scanComplete = true; //Indicate the scan is complete
      
      Serial.println("DONEEEEE");
      Serial.println(firePosition);
      
    }
    
    else {
      
      flameServo.write(fireServoPos);
      currFlameVal = analogRead(firePin);
      
      Serial.print("Position ");
      Serial.print(fireServoPos);
      Serial.print("Flame Value ");
      Serial.println(currFlameVal);
      
      if(currFlameVal < lastFlameVal) {
        
        lastFlameVal = currFlameVal;
        firePosition =  fireServoPos;
      
      }
      
      fireServoPos += 10;
      
    }
      
      lastFireTimeCount = countTime; // reset the stored time variable
    
  }
    
}

/*********************************************************************************************/
// Look For Fire function


// This function is constantly polled when navigating the maze
// it rotates the servo and if it ever sees a "flame" changes the main State 

void lookForFire(void) {
  
  int flameVal;
  
  if(countTime - lastFireTimeCount >= 15) {
    
    /* Read current fire sensor value */
    flameVal = analogRead(firePin);
    Serial.println(flameVal);
    //Serial.println(fireServoPos);
  
    flameServo.write(fireServoPos);
    
    if(servoIncreasing) {
      if(fireServoPos <= 170) {
        fireServoPos += 10; 
        flameServo.write(fireServoPos);     
      }
      else {
        servoIncreasing = 0;
      }
    }
    else {
      if(fireServoPos >= 10) {
        fireServoPos -= 10;
        flameServo.write(fireServoPos);
      }
      else {
        servoIncreasing = 1;
      }
    }
    
  
    /* If the flame is found */
    if(flameVal < 200) {
       /* Change to extiguish fire state */
       mainState = extinguishingFire;
       fireServoPos = 0; // reset the position
       lastFlameVal = 2000; // reset the last flame val
       flameServo.write(0); // write the servo back to reset position
       scanComplete = true; //Indicate the scan is complete
       Serial.println("Fire Found");
    }

    lastFireTimeCount = countTime; // reset the stored time variable

  }
  
}


