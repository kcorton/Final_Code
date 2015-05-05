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
  if(mainState == returningHome){
    if (getDis(backSonar) <= desDis) { 
      return true; 
    }

    else { 
      return false; 
    }
  }
  else{
    if (getDis(sideSonar) <= desDis) { 
      return true; 
    }

    else { 
      return false; 
    }
  }

}

/*********************************************************************************************/
// Check Side Distance function 
// if the side distance sensors sees something less than a certain distance away the function is true

boolean checkSideDisGreater(int desDis){

  if(mainState == returningHome){
    if (getDis(backSonar) >= desDis) { 
      stopAllDrive();
      return true; 
    }

    else { 
      return false; 
    }
  }
  else {
    if(getDis(sideSonar) >= desDis) { 
      stopAllDrive();
      return true; 
    }

    else { 
      return false; 
    }
  }

}

/*********************************************************************************************/
// Get Front Distance Function  
// returns the value from whichever Sonar is given

float getDis(int sonarToRead){
  switch(sonarToRead){
  case frontSonar:
    {
      return ( (567.5 * ( (float)frontEchoTime/1000000)) * 12.0);
    } 
    break;
  case sideSonar:
    {
      return ( (567.5 * ( (float)sideEchoTime/1000000)) * 12.0);
    } 
    break;
  case backSonar:
    {
      return ( (567.5 * ( (float)backEchoTime/1000000)) * 12.0);
    } 
    break;
  default:
    Serial.println("FRONT DISTANCE SONAR ERROR");
    lcd.println("ERROR 11");
    delay(5000);
    break;
  }
}

/*********************************************************************************************/
// Pings sends a ping from the sonar specified by the input parameter. If the next sonar to ping
//  is the side or back sonar, then this function then calls pulseIn() which calculates how long
//  it took for the echo to return.

void ping(int sensorToPing){
  // If waiting for an echo, dont send another ping
  if(!waiting){
    int pingPin;
    switch(sensorToPing){
    case frontSonar:
      {    //FRONTSONAR
        pingPin = frontPingPin;
        break;
      }
    case sideSonar:
      {    //SIDESONAR
        pingPin = sidePingPin;
        break;
      }
    case backSonar:
      {    //BACKSONAR
        pingPin = backPingPin;
        break;
      }
    default:
      Serial.println("HIT PING DEFAULT");
      lcd.println("ERROR");
      delay(5000);
      break;
    }

    delay(50);  //ensures a second ping is not sent out before the first reurns
    
    //send the ping
    digitalWrite(pingPin,LOW);
    digitalWrite(pingPin,HIGH);
    delayMicroseconds(250);
    digitalWrite(pingPin,LOW); 
    
    // if side sensor is pinged, use pulseIn()
    if(sensorToPing == sideSonar){
      sideEchoTime = pulseIn(sideEchoPin,HIGH);
      
      //determines which sonar to ping next
      switch(mainState){
      case findingFire:
        {
          pingNext = frontSonar;
          break;
        }
      case extinguishingFire:
        {
          pingNext = frontSonar;
          break;
        }
      case returningHome:
        {
          pingNext = frontSonar;
          break;
        }
      case madeItHome:
        {
          pingNext = frontSonar;
          break;
        }
      }
    }
    // if side sensor is pinged, use pulseIn()
    else if(sensorToPing == backSonar){
      backEchoTime = pulseIn(backEchoPin,HIGH);
      
      //determines which sonar to ping next
      switch(mainState){
      case findingFire:
        {
          pingNext = frontSonar;
          break;
        }
      case extinguishingFire:
        {
          pingNext = frontSonar;
          break;
        }
      case returningHome:
        {
          pingNext = frontSonar;
          break;
        }
      case madeItHome:
        {
          pingNext = frontSonar;
          break;
        }
      }
    }
    //otherwise (front sonar was pinged), wait for the interrupt to recieve an echo
    else{
      waiting = true;
    } 
  }
}




