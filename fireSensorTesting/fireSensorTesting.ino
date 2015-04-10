#include <Servo.h> 
 
Servo myservo;

#define firePin A0

int pos = 0;    // variable to store the servo position 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val;
  float dist;
  
  
    for(pos = 0; pos <= 180; pos += 1) // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
    
    val = analogRead(firePin);
  Serial.println(val);
  } 
  for(pos = 180; pos>=0; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
    
    val = analogRead(firePin);
  Serial.println(val);
  } 

}

// Can you put any setup and intialization info that is needed for this to work Below here 

#define extinguishingFire 1
int mainState = 0;


/*********************************************************************************************/
// Scan function 
// scans the full range of the flame sensor servo, saving positions and readings in an array, 
// and saving a global variable with the servo position at the highest flame sensing
// this is used once the flame has been seen

void scan (void) {


}

/*********************************************************************************************/
// Look For Fire function

// This function is constantly polled when navigating the maze
// it rotates the servo and if it ever sees a "flame" changes the main State 

void lookForFire (void) {
  
  // if the fire has been found 
   mainState = extinguishingFire;
  
}


