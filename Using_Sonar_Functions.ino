/*********************************************************************************************/
// Check Front Distance function 

// if the front distance sensors sees something  a certain distance away the function is true

boolean checkFrontDis(int desDis){

  if (getDis(frontSonar) <= desDis) { 
    return true; 
  }

  else { 
    return false; 
  }

}

/*********************************************************************************************/
// Check Side Distance function 

// if the side distance sensors sees something less than a certain distance away the function is true

boolean checkSideDisLess(int desDis){

  if (getDis(sideSonar) <= desDis) { 
    return true; 
  }

  else { 
    return false; 
  }

}

/*********************************************************************************************/
// Check Side Distance function 

// if the side distance sensors sees something less than a certain distance away the function is true

boolean checkSideDisGreater(int desDis){

  if (getDis(sideSonar) >= desDis) { 
    stopAllDrive();
    return true; 
  }

  else { 
    return false; 
  }

}

/*********************************************************************************************/
// Get Front Distance Function  

// returns the value from whichever Sonar is given

float getDis(int sonarToRead){
  switch(sonarToRead){
    case frontSonar:{
       return ( (567.5 * ( (float)frontEchoTime/1000000)) * 12.0);
    } 
    case sideSonar:{
       return ( (567.5 * ( (float)sideEchoTime/1000000)) * 12.0);
    } 
    case backSonar:{
       return ( (567.5 * ( (float)backEchoTime/1000000)) * 12.0);
    } 
    default:
      Serial.println("HIT EXTINGUISH FIRE DEFAULT");
      lcd.println("ERROR 11");
      delay(5000);
      break;
  }
}

/*********************************************************************************************/
// Pings all the SOnar 

void ping(int sensorToPing){
  
  
  if(!waiting){
   // Serial.println("ping");
    int pingPin;
    switch(sensorToPing){
      case frontSonar:{    //FRONTSONAR
        pingPin = frontPingPin;
        break;
      }
      case sideSonar:{    //SIDESONAR
        pingPin = sidePingPin;
        break;
      }
      case backSonar:{    //BACKSONAR
        pingPin = backPingPin;
        break;
      }
      default:
        Serial.println("HIT PING DEFAULT");
        lcd.println("ERROR");
        delay(5000);
        break;
    }
    // Serial.print(pingPin);
  
    delay(50);  //ensures a second ping is not sent out before 
                //the first returns
    digitalWrite(pingPin,LOW);
    digitalWrite(pingPin,HIGH);
    delayMicroseconds(250);
    digitalWrite(pingPin,LOW); 
   
    if(sensorToPing == backSonar){
      backEchoTime = pulseIn(backEchoPin,HIGH);
      switch(mainState){
        case findingFire:{
          pingNext = frontSonar;
          break;
        }
        case extinguishingFire:{
          pingNext = frontSonar;
          break;
        }
        case returningHome:{
          pingNext = sideSonar;
          break;
        }
        case madeItHome:{
          pingNext = frontSonar;
          break;
        }
      }
    }
    else{
      waiting = true;
    } 
  }
}



