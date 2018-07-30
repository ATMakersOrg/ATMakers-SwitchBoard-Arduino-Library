/*
 * Using a Feather Basic M0 Proto to test the SwitchBoard.
 */

#include <Streaming.h>
#include <ATMakers_SwitchBoard.h>

// looping delays
#define INTRASET_ms 300
#define INTERSET_ms 600

ATMakers_SwitchBoard sboard;

// INA219 current sense, initialized with 32V_2A calibration
const float LOW_RES_HIGH_CURRENT_MAX = 3.2;
const float LOW_RES_LOW_CURRENT_MAX  = 1.0;
const float HIGH_RES_VOLTAGE_MAX = 16.0;
const float HIGH_RES_CURRENT_MAX = 0.400;

void setup() {
  Serial.begin(9600);
  for (int s=5 ; s > 0 ; s--) {
    // give user 5 seconds to relaunch console
    if (Serial) break;
    delay(1000);
  }
  sboard.begin(5); // 5mA threshold
  sboard.resetAll();
}

void loop() {
  ATMakers_SwitchBoard::CS_Data report;
  for (int threshold=3 ; threshold <= 24 ; threshold+=3) {
    Serial << "Setting threshold to " << threshold << endl;
    sboard.currentThreshold(threshold);
    for (int set=1 ; set>=0 ; set--) {
      for (int relay=1; relay <=5; relay++) {
        String mode = (set) ? "Setting" : "Resetting";
        Serial << mode << " Relay " << relay << endl;
        (set) ? sboard.relaySet(relay) : sboard.relayReset(relay);
        delay(INTERSET_ms);
      }
      sboard.getCurrentSensorData(report);
      Serial << "Data: source="<< report.supplyvoltage_V << "V"
        << " shunt=" << report.shuntvoltage_mV << "mV"
        << " load="  << report.loadvoltage_V << "V"
        << " current=" << report.current_mA << "mA"
        << " active=" << report.current_active
        << endl;
      delay(INTRASET_ms);
    }
    delay(4000);
  }
}
