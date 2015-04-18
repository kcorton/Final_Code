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

// returns the value from the front distance sensor when called 

float getDis(int sonarNum) {

}

/*********************************************************************************************/
// Look For Fire function

// This function is constantly polled when navigating the maze
// it rotates the servo and if it ever sees a "flame" changes the main State 

void lookForFire(void) {
  
  // if the fire has been found 
   mainState = extinguishingFire;
  
}
