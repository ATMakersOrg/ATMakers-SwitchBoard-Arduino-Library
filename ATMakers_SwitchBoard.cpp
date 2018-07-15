#include "ATMakers_SwitchBoard.h"

 /*************************************************** 
  This is a library for the ATMakers SwitchBoard Feather Wing

  This Board use I2C to communicate, 2 pins are required to  
  interface
  
  Copyright ATMakers, Inc 2018
 
  MIT license, all text above must be included in any redistribution
 ****************************************************/

ATMakers_SwitchBoard::ATMakers_SwitchBoard(uint8_t addr, float limit):currentSensor(addr), currLimit(limit)
{
	//strangely these two sensors intialize differently...
	//The Current Sensor wants the full i2c address
  currentSensor.begin(INA219_ADDRESS | addr);
	//and the port exander wants just the last three bits
  portExpander.begin(addr);  
}

void ATMakers_SwitchBoard::resetAll()
{
	relayReset(0);
	relayReset(1);
	relayReset(2);
	relayReset(3);
	relayReset(4);
}
void ATMakers_SwitchBoard::relaySet(int relay)
{ 
  portExpander.digitalWrite(setMap[relay], HIGH); 
  delay(RELAY_CHANGE_DELAY); 
  portExpander.digitalWrite(setMap[relay], LOW);
}

void ATMakers_SwitchBoard::relayReset(int relay)
{ 
  portExpander.digitalWrite(resetMap[relay], HIGH); 
  delay(RELAY_CHANGE_DELAY);
  portExpander.digitalWrite(resetMap[relay], LOW);
}

boolean ATMakers_SwitchBoard::currentSensorActive(float limit)
{
  return false;
}

boolean ATMakers_SwitchBoard::currentSensorActive()
{
  return currentSensorActive(currLimit);
}

boolean ATMakers_SwitchBoard::isExtraPin(uint8_t p){
  for (int i = 0; i < 6; i++)
  {
    if (setMap[i] == p)
      return false;
    if (resetMap[i] == p)
      return false;
  }
  return true;
}


void ATMakers_SwitchBoard::pinMode(uint8_t p, uint8_t d) {
  if (isExtraPin(p))
  {
    portExpander.pinMode(p, d);
  }
}
void ATMakers_SwitchBoard::digitalWrite(uint8_t p, uint8_t d)
{
  if (isExtraPin(p))
  {
    portExpander.digitalWrite(p, d);
  }
}

void ATMakers_SwitchBoard::pullUp(uint8_t p, uint8_t d)
{
  if (isExtraPin(p))
  {
    portExpander.pullUp(p, d);
  }
}

uint8_t ATMakers_SwitchBoard::digitalRead(uint8_t p)
{
  return portExpander.digitalRead(p);
}



