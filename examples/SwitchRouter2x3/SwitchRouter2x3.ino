#include <ATMakers_SwitchBoard.h>
#include <FlashStorage.h>

/* pin configuration */
#define SPEAKER_PIN 6 /*Must be PWM*/
#define BUTTON_PIN 9 /*On Feather*/
#define LED_PIN LED_BUILTIN

/*CONFIGs*/
#define NUM_CONFIGS 3
#define RELAYS_PER_CONFIG 4
boolean configs[NUM_CONFIGS][RELAYS_PER_CONFIG] =
{
  {true, true, false, false},
  {false, false, true, true},
  {false, false, false, false}
};
int activeConfig = 0;

/*Program States*/
#define ROUTING  1
#define TUNING 2
int state = ROUTING;

/*Element States*/
#define LONG_PRESS_WAIT 1000
boolean buttonPressed = false;
int longPressTimeout = 0;

#define CURRENT_ACTIVE_WAIT 1000
boolean currentActive = false;
int currentActiveTimeout = 0;

/*Thresholds*/
//float currentLimits[NUM_CONFIGS];

/*Frequencies*/
float frequencies[] = {262, 330, 392, 523, 659, 784};

/*Declare the switchboard - could pass in address info*/
ATMakers_SwitchBoard switchBoard;

/* Data that we will store in Flash (like EEPROM on older Arduinos)*/
typedef struct ConfigData {
  boolean valid;
  float currentLimits[NUM_CONFIGS];
};
ConfigData configData;
FlashStorage(my_flash_store, ConfigData);


void setup() {
  Serial.begin(9600);

  configData = my_flash_store.read();

  /*Setup the pins*/
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  /* Read the stored current limits (if they are there*/
  if (!configData.valid)
  {
    Serial.println("No Valid Data - setting defaults");
    for (int i = 0; i < NUM_CONFIGS; i++)
    {
      configData.currentLimits[i] = 1.0;
    }
    configData.valid = true;
    my_flash_store.write(configData);
  }
  else
  {
    Serial.println("Got Valid Stored Data");
  }

  //If we have a serial port, we might as well print our current limits.
  for (int i = 0; i < NUM_CONFIGS; i++)
  {
    Serial.print("  Current Limit [");
    Serial.print(i);
    Serial.print("] -> ");
    Serial.println(configData.currentLimits[i]);
  }

  /*Start the switchboard*/
  switchBoard.begin();

  /*
    //If we are using a button pin on the SwitchBoard, this is the API
    switchBoard.pinMode(BUTTON_PIN, INPUT);
    switchBoard.pullUp(BUTTON_PIN, HIGH);
  */
  //Pin on the main board uses the Arduino call
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  if (state == ROUTING)
  {
    //First, check to see whether the button is pressed
    if (/*switchBoard.*/digitalRead(BUTTON_PIN) == LOW)
    {
      if (!buttonPressed) //First time through
      {
        buttonPressed = true;
        longPressTimeout = millis() + LONG_PRESS_WAIT;
      }
      else
      {
        //have we been held long enough?
        if (millis() > longPressTimeout)
        {
          Serial.println("Long Press Detected");

          tone(SPEAKER_PIN, 1000, 500);
          state = TUNING; //state changed - don't finish the loop()
          return;
        }
      }
    }
    else //button is not pressed
    {
      if (buttonPressed) //digital read = HIGH
      {
        Serial.println("Button Released (short)");
        buttonPressed = false;
        nextConfig(); //switch to the next configuration
      }
    }

    //now, check to see if the current sensor is detecting a press...
    float current = switchBoard.getCurrent_mA();
    if (abs(current) > abs(configData.currentLimits[activeConfig]))
    {
      if (!currentActive) //First time through
      {
        currentActive = true;
        currentActiveTimeout = millis() + CURRENT_ACTIVE_WAIT;
        //We might put an LED blink here?
      }
      else
      {
        //have we been held long enough?
        if (millis() > currentActiveTimeout)
        {
          nextConfig();
          currentActive = false;
          currentActiveTimeout = 0;
          return;
        }
      }
    }
    else
    {
      currentActive = false;
      currentActiveTimeout = 0;
    }
  }
  else if (state == TUNING)
  {
    if (/*switchBoard.*/digitalRead(BUTTON_PIN) == HIGH) //button released
    {
      buttonPressed = false;
    }
    //while we're tuning, we don't have to do anything unless they press the button again
    else if ( !buttonPressed && /*switchBoard.*/digitalRead(BUTTON_PIN) == LOW)
    {
      Serial.println("Button Down in TUNING");
      float newActiveCurrent = switchBoard.getCurrent_mA();
      configData.currentLimits[activeConfig] = newActiveCurrent * .5;
      my_flash_store.write(configData);
      Serial.print("Current through config #");
      Serial.print(activeConfig);
      Serial.print(" = " );
      Serial.print(newActiveCurrent);
      Serial.println();
      tone(SPEAKER_PIN, 1000, 1000);
      delay(500);
      state = ROUTING;
    }
    Serial.print("CURRENT SENSOR: ");
    Serial.print(switchBoard.getCurrent_mA());
    Serial.println();
  }
}

void nextConfig()
{
  //switch to the next configuration
  activeConfig = (activeConfig + 1) % NUM_CONFIGS;
  Serial.print("Switching to config #");
  Serial.println(activeConfig);
  for (int i = 0 ; i < 4; i++)
  {
    if (configs[activeConfig][i] == true)
      switchBoard.relaySet(i + 1);
    else
      switchBoard.relayReset(i + 1);
  }
  tone(SPEAKER_PIN, frequencies[activeConfig], 500);
}

