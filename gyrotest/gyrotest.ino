/*
The sensor outputs provided by the library are the raw 16-bit values
obtained by concatenating the 8-bit high and low gyro data registers.
They can be converted to units of dps (degrees per second) using the
conversion factors specified in the datasheet for your particular
device and full scale setting (gain).

Example: An L3GD20H gives a gyro X axis reading of 345 with its
default full scale setting of +/- 245 dps. The So specification
in the L3GD20H datasheet (page 10) states a conversion factor of 8.75
mdps/LSB (least significant bit) at this FS setting, so the raw
reading of 345 corresponds to 345 * 8.75 = 3020 mdps = 3.02 dps.
*/

#include <L3G.h>
#include <Wire.h>
#include <TimerOne.h>

#define turningSpeed 400

L3G gyro;

int volatile long countTime = 0;
int currGyroReading;
int turnComplete = false;
int mdps = 0;
float totalDegrees = 0;
long lastTurnCount = 0;
long lastTurnTime = 0;  
long currTurnTime = 0;
int offset = 0;

/* Speeds of the wheels in encoder ticks per second */
int baseSpeed = 980; //encoder ticks per second
int leftSpeed = baseSpeed;
int rightSpeed = baseSpeed;

void setup() {
  
  Serial.begin(9600);
  
  Timer1.initialize(100000); // Interrupt every 0.1 second
  Timer1.attachInterrupt(TimerISR);
  
  I2C_Init();
  Gyro_Init();   
  
  initializeMotors();
  
  calcGyroOffset();
  
}

void TimerISR(void) {
  
  countTime++;
  
}

void loop() {
  
  turn(-30);
  if(turnComplete) {
    lastTurnTime = 0;
    totalDegrees = 0;
    turnComplete = false;
    Serial.println("DONE");
    lastTurnTime = millis();
    delay(5000);
  }

}


void calcGyroOffset(void) {
  
  int i;
  long totalGyroReading = 0;
  
  for(i = 0; i < 1000; i++) {
    
    gyro.read();
    totalGyroReading += (int)gyro.g.z;
    
    delay(10);
    
  }
  
  Serial.println("OFFSET");
  offset = (float)totalGyroReading/1000;
  Serial.println(offset);
  
}
    
/* Positive turn turns left, negative turns right */
void turn(int turnDeg) {
  
  /* If enough time has passed */
  if(countTime - lastTurnCount >= 1) {
    
    currTurnTime = millis() - lastTurnTime;
  
    gyro.read();
    currGyroReading = (int)gyro.g.z;
    mdps = (currGyroReading - offset);
    mdps = (float)mdps * 0.07;
    
    totalDegrees += (float)mdps * ((float)currTurnTime/2000) * 2.1;
  
    if(turnDeg >= 0) {
      
      /* Write Changes to the motors */
      leftSpeed = -turningSpeed;
      rightSpeed = turningSpeed;
      updateMotors();
      
      if(totalDegrees >= turnDeg) {
        turnComplete = true;
        
        /* Stop the motors */
        leftSpeed = 0;
        rightSpeed = 0;
        updateMotors();
        
      }
    }
    
    else if(turnDeg < 0) {
      
      /* Write Changes to the motors */
      leftSpeed = turningSpeed;
      rightSpeed = -turningSpeed;
      updateMotors();
      
      if(totalDegrees <= turnDeg) {
        turnComplete = true;
        
        /* Stop the motors */
        leftSpeed = 0;
        rightSpeed = 0;
        updateMotors();
      }
    }
    
    Serial.println(totalDegrees);
    
    lastTurnTime = millis();
    lastTurnCount = countTime;
    
  }
  
}

void I2C_Init()
{
  Wire.begin();
}

void Gyro_Init()
{
  if (!gyro.init())
  {
    Serial.println("Failed to autodetect gyro type!");
    while (1);
  }
  gyro.enableDefault();
  gyro.writeReg(L3G::CTRL_REG4, 0x20); // 2000 dps full scale
  gyro.writeReg(L3G::CTRL_REG1, 0x0F); // normal power mode, all axes enabled, 100 Hz
}
