#include <ATMakers_SwitchBoard.h>

/* pin configuration */
#define SPEAKER_PIN 10 /*Must be PWM*/
#define BUTTON_PIN 12 /*Must be PWM*/
#define LED_PIN LED_BUILTIN

/*CONFIGs*/
#define NUM_CONFIGS 3
#define RELAYS_PER_CONFIG 4
boolean configs[NUM_CONFIGS][RELAYS_PER_CONFIG] =
{ {false, false, false, false},
  {true, false, true, false},
  {true, true, true, true}
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
float currentLimits[NUM_CONFIGS];

/*Frequencies*/
float frequencies[] = {262, 330, 392, 523, 659, 784};

/*Declare the switchboard - could pass in address info*/
ATMakers_SwitchBoard switchBoard;

void setup() {
  /*Setup the pins*/
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  for (int i = 0; i < NUM_CONFIGS; i++)
  {
    currentLimits[i] = 1.0;
  }

  /*Start the switchboard*/
  switchBoard.begin();
}

void loop() {

  if (state == ROUTING)
  {
    //First, check to see whether the button is pressed
    if (digitalRead(BUTTON_PIN) == LOW)
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
        buttonPressed = false;
        nextConfig(); //switch to the next configuration
      }
    }

    //now, check to see if the current sensor is detecting a press...
    float current = switchBoard.getCurrent_mA();
    if (current > currentLimits[activeConfig])
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
    //while we're tuning, we don't have to do anything unless they press the button again
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      float newActiveCurrent = switchBoard.getCurrent_mA();
      currentLimits[activeConfig] = newActiveCurrent;
      tone(SPEAKER_PIN, 1000, 1000);
      state = ROUTING;
    }
  }
}

void nextConfig()
{
  //switch to the next configuration
  activeConfig = (activeConfig + 1) % NUM_CONFIGS;
  for (int i = 0 ; i < 4; i++)
  {
    if (configs[activeConfig][i] == true)
      switchBoard.relaySet(i + 1);
    else
      switchBoard.relayReset(i + 1);
  }
  tone(SPEAKER_PIN, frequencies[activeConfig], 500);
}

