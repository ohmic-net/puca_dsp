
#include "set_module.h"
#include "set_settings.h"
#include "set_codec.h"

//controller_module myPedal definition
controller_module *myPedal = new controller_module();

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~ DSP Class Definitions (bsdsp files) ~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  fractionalDelay delay1;
  fractionalDelay delay2;
  bool x = delay1.init(3); //init for 3 ms delay
  bool y = delay2.init(3); //init for 3 ms delay
  oscillator lfo1;
  oscillator lfo2;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~ CONTROLLER MODULE CLASS DEFINITIONS ~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// Define the ControllerModule functions declared above
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void controller_module::init()  //effect module class initialization
 {
  name = "Stereo Chorus";
  inputMode = IM_LR;   // IM_LR or IM_LMIC

  // Set up pin Modes for the switches and LEDs
  // For mode details, see control_task() and button_task() in task.cpp
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(KEY1, INPUT_PULLUP);  
  pinMode(KEY2, INPUT_PULLUP); 
  pinMode(KEY3, INPUT_PULLUP);
  pinMode(KEY4, INPUT_PULLUP);        
 
  //setting up the buttons
  button[0].name = "KEY1";
  button[0].mode = BM_MOMENTARY;
  button[0].pin = KEY1;

  button[1].name = "KEY2";
  button[1].mode = BM_TOGGLE;
  button[1].pin = KEY2;

  //add gain control
  control[0].name = "Rate";
  control[0].mode = CM_POT;
  control[0].levelCount = 128;
  control[0].pin = POT1;

  //add range control
  control[1].name = "Depth";
  control[1].mode = CM_POT;
  control[1].levelCount = 128;
  control[1].pin = POT2;

  control[2].name = "F/P Diff";
  control[2].mode = CM_POT;
  control[2].levelCount = 128;
  control[2].pin = POT3;

  // control[3].name = "Input Mode";
  // control[3].mode = CM_SELECTOR;
  // control[3].levelCount = 2;  //0:mono 1:stereo
  // control[3].pin = POT4;

  freq = 5;
  depth = 0.5;
  beatFrequency = 2.5;
  phaseDiff = 0;
  stereo = 1;
  asynch = 0;
  lfo1.setFrequency(freq);
  lfo2.setFrequency(freq+beatFrequency);
 }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void controller_module::onButtonChange(int buttonIndex)
{
  switch(buttonIndex)
  {
    case 0: //main button state has changed
    {
      if(button[0].value) //if effect is activated
      {
        codec.loopback(0);
        //codec.hpVolSet(40, 40);
        digitalWrite(LED1, HIGH);
      }
      else //if effect is bypassed
      {
        codec.loopback(1);
        //codec.hpVolSet(0, 0);
        digitalWrite(LED1, LOW);
      }
      break;
    }
    case 1: //the button[1] state has changed
    {
      if(button[1].value) // just test LED and Switch
      {digitalWrite(LED2, HIGH);}
      else 
      {digitalWrite(LED2, LOW);}
      break;
    }
  }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void controller_module::onControlChange(int controlIndex)
{
  switch(controlIndex)
  {
    case 0: //rate
    {
      freq = 0.5 + 10 * (float)control[0].value/127.0;
      lfo1.setFrequency(freq);
      lfo2.setFrequency(freq + beatFrequency);
      break;
    }
    case 1:  //depth
    {
      depth = 1.49 * (float)control[1].value/127.0;
      break;
    }
    case 2:  //phase or frequency difference
    {
      beatFrequency = 5 * (float)control[2].value/127.0;
      phaseDiff = (float)control[2].value;
      lfo2.setFrequency(freq + beatFrequency);
      break;
    }
    case 3:  //stereo
    {
      stereo = (bool)control[3].value;
      break;
    }
 }
}
