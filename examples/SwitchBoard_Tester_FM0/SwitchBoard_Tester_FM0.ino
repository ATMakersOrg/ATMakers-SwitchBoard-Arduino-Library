/*
 * Using a Feather Basic M0 Proto to test the SwitchBoard.
 */

#include <Streaming.h>
#include <ATMakers_SwitchBoard.h>

// looping delays
#define INTRASET_ms 300
#define INTERSET_ms 600

ATMakers_SwitchBoard sboard;

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
      Serial << "Data: source="<< _FLOAT(report.supplyvoltage_V,3) << "V"
        << " shunt=" << _FLOAT(report.shuntvoltage_mV,3) << "mV"
        << " load="  << _FLOAT(report.loadvoltage_V,3) << "V"
        << " current=" << _FLOAT(report.current_mA,3) << "mA"
        << " active=" << report.current_active
        << endl;
      delay(INTRASET_ms);
    }
    delay(4000);
  }
}
