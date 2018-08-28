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
#define RELAY_CHANGE_DELAY   20

class ATMakers_SwitchBoard {
private:
  Adafruit_MCP23017 portExpander;
  Adafruit_INA219   currentSensor;
  TwoWire* _i2c;
  const int setMap[6]   = {-1,0,1,2,3,4};
  const int resetMap[6] = {-1,8,9,10,11,12};
  float currentThreshold_mA;
public:  
  void begin(float currThreshold_mA=1.0, uint8_t board_addr=0); // 0-3 depending on A1,A0 mods. Masked to 2 ls-bits
  void relaySet(int relay);
  void relayReset(int relay);
  void resetAll();
  
  float currentThreshold(void) { return currentThreshold_mA; };   // get the existing current threshold (mA)
  float currentThreshold(float new_threshold_mA); // set a new current threshold, returns the old setting

  void pinMode(uint8_t p, uint8_t d);
  void digitalWrite(uint8_t pin, uint8_t d);
  void pullUp(uint8_t p, uint8_t d);
  uint8_t digitalRead(uint8_t pin);

  boolean isExtraPin(uint8_t p);
  boolean currentSensorActive(void);

  // current sensor report structure
  struct CS_Data {
    float supplyvoltage_V;
    float shuntvoltage_mV;
    float loadvoltage_V;
    float current_mA;
    bool  current_active;
  };

  void getCurrentSensorData(CS_Data& report);

  // expose INA219 methods
  inline void  setCalibration_32V_2A   (void) { currentSensor.setCalibration_32V_2A(); } 
  inline void  setCalibration_32V_1A   (void) { currentSensor.setCalibration_32V_1A(); }
  inline void  setCalibration_16V_400mA(void) { currentSensor.setCalibration_16V_400mA(); }
  // TI's bus voltage is the load voltage at V-, the low side of the shunt resistor - hide after testing
  // inline float getBusVoltage_V(void) { return currentSensor.getBusVoltage_V(); }
  inline float getShuntVoltage_mV(void) { return currentSensor.getShuntVoltage_mV(); }
  inline float getCurrent_mA(void) { return currentSensor.getCurrent_mA(); }

  // new voltage functions to cope with semantics issues
  // Supply voltage at V+, Load voltage at V-
  inline float getSupplyVoltage_V(void) { return (getLoadVoltage_V() + (getShuntVoltage_mV()/1000.0)); }
  inline float getLoadVoltage_V(void)   { return currentSensor.getBusVoltage_V(); }
};

/****************************************************************************
 * INA219 Notes:
 *
 * Figure 13 in TI's INA219 datasheet depicts the Power Bus voltage at V+
 * and Load voltage at V- which is consistent with other high-side current
 * measurement diagrams.
 * However, the datasheet explicitly states "The bus voltage for the INA219
 * is internally measured at the VIN– pin to measure the voltage level
 * delivered to the load." The makes it easier to compute the load power,
 * but you might want to read the high-side source voltage, e.g. for a
 * battery. Hence the supplemental methods.
 ****************************************************************************/

#endif
