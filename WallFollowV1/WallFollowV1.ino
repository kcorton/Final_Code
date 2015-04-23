#include <TimerOne.h>

#define FRONTSONAR 1
#define SIDESONAR 2
#define BACKSONAR 3

int fanPin = 22;

long countTime = 0;

//==========================================================================
void setup(){
  Serial.begin(9600);
  
  //timer set up
  Timer1.initialize(100000);	//0.1 seconds               
  Timer1.attachInterrupt(timerISR);
  
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin,LOW);
  
  initializeMotors();
  
  initializeSonar();
}
//==========================================================================
void loop(){
  followWall();
  ping(getNextPing());
  Serial.println(getDis(SIDESONAR));
}
//==========================================================================
void timerISR(void) {
  incrementAccelTime();
  countTime++;
}
