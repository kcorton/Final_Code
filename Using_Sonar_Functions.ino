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

// if the side distance sensors sees something  a certain distance away the function is true

boolean checkSideDis(int desDis){

  if (getDis(sideSonar) <= desDis) { 
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
  }
}

// Pings all the SOnar 

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
    // Serial.print(pingPin);
  
    delay(50);  //ensures a second ping is not sent out before 
                //the first returns
    digitalWrite(pingPin,LOW);
    digitalWrite(pingPin,HIGH);
    delayMicroseconds(250);
    digitalWrite(pingPin,LOW); 
   
    waiting = true; 
  }
}



