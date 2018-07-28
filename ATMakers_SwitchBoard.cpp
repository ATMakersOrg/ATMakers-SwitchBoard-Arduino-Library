#include "ATMakers_SwitchBoard.h"

 /*************************************************** 
  This is a library for the ATMakers SwitchBoard Feather Wing

  This Board use I2C to communicate, 2 pins are required to  
  interface
  
  Copyright ATMakers, Inc 2018
 
  MIT license, all text above must be included in any redistribution
 ****************************************************/


void ATMakers_SwitchBoard::begin()
{
	//strangely these two sensors intialize differently...
	//The Current Sensor wants the full i2c address
  currentSensor.begin(INA219_ADDRESS | addr);
  currentSensor.setCalibration_16V_400mA();
	//and the port exander wants just the last three bits
  portExpander.begin();

  portExpander.pinMode(0, OUTPUT);
  portExpander.pinMode(8, OUTPUT);
  portExpander.pinMode(1, OUTPUT);
  portExpander.pinMode(9, OUTPUT);
  portExpander.pinMode(2, OUTPUT);
  portExpander.pinMode(10, OUTPUT);
  portExpander.pinMode(3, OUTPUT);
  portExpander.pinMode(11, OUTPUT);
  portExpander.pinMode(4, OUTPUT);
  portExpander.pinMode(12, OUTPUT);
  


}
void ATMakers_SwitchBoard::resetAll()
{
	relayReset(1);
	relayReset(2);
	relayReset(3);
	relayReset(4);
	relayReset(5);
}
void ATMakers_SwitchBoard::relaySet(int relay)
{ 
  portExpander.pinMode(setMap[relay], OUTPUT);
  portExpander.digitalWrite(setMap[relay], HIGH); 
  delay(RELAY_CHANGE_DELAY); 
  portExpander.digitalWrite(setMap[relay], LOW);
}

void ATMakers_SwitchBoard::relayReset(int relay)
{ 
  portExpander.pinMode(resetMap[relay], OUTPUT);
  portExpander.digitalWrite(resetMap[relay], HIGH); 
  delay(RELAY_CHANGE_DELAY); 
  portExpander.digitalWrite(resetMap[relay], LOW);
}

boolean ATMakers_SwitchBoard::currentSensorActive(float limit)
{
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;

  shuntvoltage = currentSensor.getShuntVoltage_mV();
  busvoltage = currentSensor.getBusVoltage_V();
  current_mA = currentSensor.getCurrent_mA();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.println("");
  
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



