/*************************************************** 
  This is a library for the ATMakers SwitchBoard Feather Wing

  This Board use I2C to communicate, 2 pins are required to  
  interface
  
  Copyright ATMakers, Inc 2018
 
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ATMAKERS_SWITCHBOARD_H_
#define _ATMAKERS_SWITCHBOARD_H_
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_INA219.h>

#define CURRENT_SENSOR_DELAY 20
#define RELAY_CHANGE_DELAY 20


	 
class ATMakers_SwitchBoard {
  Adafruit_MCP23017 portExpander;
  Adafruit_INA219 currentSensor;
  const int setMap[6] = {-1,0,1,2,3,4};
  const int resetMap[6] = {-1,8,9,10,11,12};
  float currLimit = 1.0;
  uint8_t addr = 0x00;
public:  
//  ATMakers_SwitchBoard(uint8_t addr=0x00, float limit = 1.0);
  void begin();
	void relaySet(int relay);
	void relayReset(int relay);
	void resetAll();

  void pinMode(uint8_t p, uint8_t d);
  void digitalWrite(uint8_t pin, uint8_t d);
  void pullUp(uint8_t p, uint8_t d);
  uint8_t digitalRead(uint8_t pin);

  boolean isExtraPin(uint8_t p);
  
  boolean currentSensorActive(float currLimit);
  boolean currentSensorActive();
  
};


#endif
