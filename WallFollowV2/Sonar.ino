//Contains all functions pertaining to running/using the sonar
//sonar pins
int frontPingPin = 27;
int sidePingPin = 28;
int backPingPin = 29;
int frontEchoPin = 18;
int sideEchoPin = 19;
int backEchoPin = 26;

long frontPingTime = 0;
long sidePingTime = 0;
long backPingTime = 0;

volatile long frontEchoTime = 0;
volatile long sideEchoTime = 0;
volatile long backEchoTime = 0;

int tempEchoFront = 0;
int tempEchoSide = 0;
int tempEchoBack = 0;

boolean waiting = false;

int pingNext = FRONTSONAR;

#define findingFire 0
#define exstinguishingFire 1
#define returningHome 2
#define arrivedHome 3
int mainState = 0;
//========================================================
void initializeSonar(){
  //Sonar pin declarations
  pinMode(frontPingPin,OUTPUT);
  pinMode(frontEchoPin,INPUT);
  pinMode(sidePingPin,OUTPUT);
  pinMode(sideEchoPin,INPUT);
  pinMode(backPingPin,OUTPUT);
  pinMode(backEchoPin,INPUT);
  
  //Sonar interrupt initialization
  attachInterrupt(5,frontSonarISR,CHANGE); 
  attachInterrupt(4,sideSonarISR,CHANGE);  
//  attachInterrupt(3,backSonarISR,CHANGE);  
  
  //Ensures all Soonar pin pins are initialized to low
  digitalWrite(frontPingPin,LOW);
  digitalWrite(sidePingPin,LOW);
  digitalWrite(backPingPin,LOW);
}
int getNextPing(){
  return pingNext; 
}
//=======================================================
float getDis(int sonarToRead){
  switch(sonarToRead){
    case FRONTSONAR:{
       return ( (567.5 * ( (float)frontEchoTime/1000000)) * 12.0);
    } 
    case SIDESONAR:{
       return ( (567.5 * ( (float)sideEchoTime/1000000)) * 12.0);
    } 
    case BACKSONAR:{
       return ( (567.5 * ( (float)backEchoTime/1000000)) * 12.0);
    } 
  }
}
//========================================================
void ping(int sensorToPing){
  if(!waiting){
    int pingPin;
    switch(sensorToPing){
      case FRONTSONAR:{
        pingPin = frontPingPin;
        break;
      }
      case SIDESONAR:{
        pingPin = sidePingPin;
        break;
      }
      case BACKSONAR:{
        pingPin = backPingPin;
        break;
      }
    }
      // Serial.print(pingPin);
  
    delay(50);  //ensures a second ping is not sent out before 
                //the first returns
    digitalWrite(pingPin,LOW);
    digitalWrite(pingPin,HIGH);
    delayMicroseconds(250);
    digitalWrite(pingPin,LOW); 
   
    if(sensorToPing == BACKSONAR){
      backEchoTime = pulseIn(backEchoPin,HIGH);
      switch(mainState){
        case findingFire:{
          pingNext = FRONTSONAR;
          break;
        }
        case exstinguishingFire:{
          pingNext = FRONTSONAR;
          break;
        }
        case returningHome:{
          pingNext = SIDESONAR;
          break;
        }
        case arrivedHome:{
          pingNext = FRONTSONAR;
          break;
        }
      }
    }
    else{
      waiting = true;
    }
    
  }
}
//========================================================
void frontSonarISR(){
  if(digitalRead(frontEchoPin) == HIGH){
    frontPingTime = micros();
  }
  else{
    tempEchoFront = micros() - frontPingTime;
    if(tempEchoFront < 30000){
      frontEchoTime = tempEchoFront;
      switch(mainState){
        case findingFire:{
          pingNext = SIDESONAR;
          break;
        }
        case exstinguishingFire:{
          pingNext = FRONTSONAR;
          break;
        }
        case returningHome:{
          pingNext = SIDESONAR;
          break;
        }
        case arrivedHome:{
          pingNext = FRONTSONAR;
          break;
        }
      }
    }
    else{
      pingNext = FRONTSONAR; 
    }
    waiting = false; 
  }
}
//========================================================
void sideSonarISR(){
  if(digitalRead(sideEchoPin) == HIGH){
    sidePingTime = micros();
  }
  else{
    tempEchoSide = micros() - sidePingTime;
    if(tempEchoSide < 30000){
      sideEchoTime = tempEchoSide;
      switch(mainState){
        case findingFire:{
          pingNext = FRONTSONAR;
          break;
        }
        case exstinguishingFire:{
          pingNext = FRONTSONAR;
          break;
        }
        case returningHome:{
          pingNext = BACKSONAR;
          break;
        }
        case arrivedHome:{
          pingNext = FRONTSONAR;
          break;
        }
      }
    }
    else{
      pingNext = SIDESONAR; 
    }
    waiting = false;
  }
}
//========================================================
//void backSonarISR(){
//  if(digitalRead(backEchoPin) == HIGH){
//    backPingTime = micros();
//  }
//  else{
//    tempEchoBack = micros() - backPingTime;
//    if(tempEchoBack < 30000){
//      backEchoTime = tempEchoBack;
//      pingNext = FRONTSONAR;
//    }
//    else{
//      pingNext = FRONTSONAR; 
//    }
//    waiting = false;
//  }
//}
