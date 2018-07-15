#include <ATMakers_SwitchBoard.h>

ATMakers_SwitchBoard switchboard(0x00, 1.0);

struct Relay {
  int no1;
  int com1;
  int nc1;
  int no2;
  int com2;
  int nc2;
};

Relay relays[6];

int  currentPos = 21;

int  led1 = A10;
int  led2 = A11;

int  startBut = 4;

void setup() {
  Serial.begin(9600);
  //first, set all of the relays to "off"

  switchboard.resetAll();

  //RELAY NUMBERS are 1-6 (not 0-5) to match the board silkscreen
  //map relays to pins
  relays[1].no1 = 0;
  relays[1].com1 = 1;
  relays[1].nc1 = 2;
  relays[1].no2 = 3;
  relays[1].com2 = 1;
  relays[1].nc2 = 5;

  relays[2].no1 = 6;
  relays[2].com1 = 7;
  relays[2].nc1 = 8;
  relays[2].no2 = 9;
  relays[2].com2 = 7;
  relays[2].nc2 = 11;

  relays[3].no1 = 24;
  relays[3].com1 = 25;
  relays[3].nc1 = 26;
  relays[3].no2 = 27;
  relays[3].com2 = 25;
  relays[3].nc2 = 29;

  relays[4].no1 = 30;
  relays[4].com1 = 31;
  relays[4].nc1 = 32;
  relays[4].no2 = 33;
  relays[4].com2 = 31;
  relays[4].nc2 = 35;

  relays[5].no1 = 36;
  relays[5].com1 = 37;
  relays[5].nc1 = 38;
  relays[5].no2 = 39;
  relays[5].com2 = 37;
  relays[5].nc2 = 15;

  for (int i = 0; i < 5; i++)
  {
    pinMode(relays[i].no1, INPUT);
    pinMode(relays[i].com1, OUTPUT);
    pinMode(relays[i].nc1, INPUT);
    digitalWrite(relays[i].com1, LOW);
    pinMode(relays[i].no2, INPUT);
    pinMode(relays[i].com2, OUTPUT);
    pinMode(relays[i].nc2, INPUT);
    digitalWrite(relays[i].com2, LOW);
  }

  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
  pinMode(led2, OUTPUT);
  digitalWrite(led2, LOW);

  pinMode(startBut, INPUT_PULLUP);

  int  currentPos = 21;

  //this might need PWM or the like
  pinMode(currentPos, OUTPUT);
  digitalWrite(currentPos, LOW);

}

void loop() {
  if (digitalRead(startBut) == LOW)
  {
    runTest();
  }
  delay(100);
}
void runTest()
{
  boolean failed = false;
  for (int i = 1; i < 6; i++)
  {
    boolean result = testRelay(i);
    if (result)
      {
        Serial.print("Relay #"); Serial.print(i); Serial.println(" PASSED!");
      }
    else
    {
        failed = true;
    }
  }
  if (failed)
  {
    Serial.println("FAILED SOME TEST");
//    pinMode(led1, OUTPUT);
//    digitalWrite(led1,HIGH);
//    pinMode(led2, INPUT);
//    digitalWrite(led2,HIGH);
  }
  else
  {
    Serial.println("PASSED ALL TEST");
  }
  
}
/*
   relanNumber is 1-6
*/
boolean testRelay(int relayNumber)
{
  Relay r = relays[relayNumber];
  boolean failed = false;

  Serial.print("\nTesting Relay #");
  Serial.println(relayNumber);

  //test with relay "Reset"
  Serial.println("\nTurning off relay");
  switchboard.relayReset(relayNumber);
  //test with com1 (and therefore com2 on this board) low
  setPullups(r, INPUT_PULLUP);
  Serial.println("Setting COM LOW");
  digitalWrite(r.com1, LOW);
  if (!checkPin(r.nc1, LOW, "NC1 is not connected while relay off and LOW")) failed = true;
  if (!checkPin(r.nc2, LOW, "NC2 is not connected while relay off and LOW")) failed = true;
  if (!checkPin(r.no1, HIGH, "NO1 is connected while relay off and LOW")) failed = true;
  if (!checkPin(r.no2, HIGH, "NO2 is connected while relay off and LOW")) failed = true;

  setPullups(r, INPUT_PULLDOWN);
  Serial.println("Setting COM HIGH");
  digitalWrite(r.com1, HIGH);
  if (!checkPin(r.nc1, HIGH, "NC1 is not connected while relay off and HIGH")) failed = true;
  if (!checkPin(r.nc2, HIGH, "NC2 is not connected while relay off and HIGH")) failed = true;
  if (!checkPin(r.no1, LOW, "NO1 is connected while relay off and HIGH")) failed = true;
  if (!checkPin(r.no2, LOW, "NO2 is connected while relay off and HIGH")) failed = true;
  digitalWrite(r.com1, LOW);

  //test with relay "Set"
  Serial.println("\nTurning on relay");
  switchboard.relaySet(relayNumber);

  //test with com1 (and therefore com2 on this board) low
  setPullups(r, INPUT_PULLUP);
  Serial.println("Setting COM LOW");
  digitalWrite(r.com1, LOW);
  if (!checkPin(r.nc1, HIGH, "NC1 is connected while relay on and LOW")) failed = true;
  if (!checkPin(r.nc2, HIGH, "NC2 is connected while relay on and LOW")) failed = true;
  if (!checkPin(r.no1, LOW, "NO1 is not connected while relay on and LOW")) failed = true;
  if (!checkPin(r.no2, LOW, "NO2 is not connected while relay on and LOW")) failed = true;

  setPullups(r, INPUT_PULLDOWN);
  Serial.println("Setting COM HIGH");
  digitalWrite(r.com1, HIGH);
  if (!checkPin(r.nc1, LOW, "NC1 not connected while relay on and HIGH")) failed = true;
  if (!checkPin(r.nc2, LOW, "NC2 not connected while relay on and HIGH")) failed = true;
  if (!checkPin(r.no1, HIGH, "NO1 is not connected while relay on and HIGH")) failed = true;
  if (!checkPin(r.no2, HIGH, "NO2 is not connected while relay on and HIGH")) failed = true;
  digitalWrite(r.com1, LOW);


    return !failed;
}

void setPullups(Relay &r, int mode)
{
  pinMode(r.nc1, mode);
  pinMode(r.nc2, mode);
  pinMode(r.no1, mode);
  pinMode(r.no2, mode);
}

boolean checkPin(int pin, int val, char* msg)
{
  if (digitalRead(pin) != val)
  {
    Serial.print("ERROR: ");
    Serial.print(msg);
    Serial.print(" (pin = ");
    Serial.print(pin);
    Serial.println(")");
    return false;
  }
  return true;
}


