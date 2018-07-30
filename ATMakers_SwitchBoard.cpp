#include "ATMakers_SwitchBoard.h"
#include <Streaming.h>

 /***************************************************
  This is a library for the ATMakers SwitchBoard Feather Wing

  This Board use I2C to communicate, 2 pins are required to
  interface

  Copyright ATMakers, Inc 2018

  MIT license, all text above must be included in any redistribution
 ****************************************************/


void ATMakers_SwitchBoard::begin(float p_current_Threshold_mA, uint8_t board_addr)
{
  currentThreshold_mA = p_current_Threshold_mA;
  board_addr &= 0b0011; // mask to A1 A0
  //strangely these two sensors initialize differently...
  //The Current Sensor wants the full i2c address
  currentSensor.begin(INA219_ADDRESS | board_addr);
  //and the port expander wants just the last three bits
  portExpander.begin(board_addr);

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

float ATMakers_SwitchBoard::currentThreshold(float new_threshold_mA) {
  float oldval = currentThreshold_mA;
  currentThreshold_mA = new_threshold_mA;
  return oldval;
}

boolean ATMakers_SwitchBoard::currentSensorActive(void)
{
  return (currentSensor.getCurrent_mA() > currentThreshold_mA);
}

void ATMakers_SwitchBoard::getCurrentSensorData(CS_Data& report)
{
  report.shuntvoltage_mV = currentSensor.getShuntVoltage_mV();
  report.loadvoltage_V   = getLoadVoltage_V();
  report.supplyvoltage_V = report.loadvoltage_V + (report.shuntvoltage_mV/1000.0);
  report.current_mA      = currentSensor.getCurrent_mA();
  report.current_active  = (report.current_mA > currentThreshold_mA);
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
