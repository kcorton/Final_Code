//Contains all functions pertaining to running/using the sonar
//sonar pins
int frontPingPin = 27;
int sidePingPin = 28;
int backPingPin = 29;
int frontEchoPin = 18;
int sideEchoPin = 19;
int backEchoPin = 20;

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
  attachInterrupt(3,backSonarISR,CHANGE);  
  
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
      case 1:{    //FRONTSONAR
        pingPin = frontPingPin;
        break;
      }
      case 2:{    //SIDESONAR
        pingPin = sidePingPin;
        break;
      }
      case 3:{    //BACKSONAR
        pingPin = backPingPin;
        break;
      }
    }
  
    delay(50);  //ensures a second ping is not sent out before 
                //the first returns
    digitalWrite(pingPin,LOW);
    digitalWrite(pingPin,HIGH);
    delayMicroseconds(250);
    digitalWrite(pingPin,LOW); 
   
    waiting = true; 
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
      pingNext = SIDESONAR;
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
      pingNext = BACKSONAR;
    }
    else{
      pingNext = SIDESONAR; 
    }
    waiting = false;
  }
}
//========================================================
void backSonarISR(){
  if(digitalRead(backEchoPin) == HIGH){
    backPingTime = micros();
  }
  else{
    tempEchoBack = micros() - backPingTime;
    if(tempEchoBack < 30000){
      backEchoTime = tempEchoBack;
      pingNext = FRONTSONAR;
    }
    else{
      pingNext = BACKSONAR; 
    }
    waiting = false;
  }
}
