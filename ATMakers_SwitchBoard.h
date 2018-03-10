/*************************************************** 
  This is a library for the ATMakers SwitchBoard Feather Wing

  This Board use I2C to communicate, 2 pins are required to  
  interface
  
  Copyright ATMakers, Inc 2018
 
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ATMAKERS_SWITCHBOARD_H_
#define _ATMAKERS_SWITCHBOARD_H_

#include <Adafruit_MCP23017.h>
#include <Adafruit_INA219.h>

#define CURRENT_SENSOR_DELAY 10

int setMap[] = {-1,7,5,3,15,13};
int resetMap[] = {-1,6,4,2,14,12};

	 
class ATMakers_SwitchBoard : public Adafruit_MCP23017, public Adafruit_INA219{
public:
	
	inline void relaySet(int relay) { digitalWrite(setMap[relay], HIGH); delay(CURRENT_SENSOR_DELAY); digitalWrite(setMap[relay], LOW);}
	inline void relayReset(int relay) { digitalWrite(resetMap[relay], HIGH); delay(CURRENT_SENSOR_DELAY);digitalWrite(resetMap[relay], LOW);}
	
	boolean currentSensorActive(float thresholdMA=1.0);
	
private:



};


#endif
