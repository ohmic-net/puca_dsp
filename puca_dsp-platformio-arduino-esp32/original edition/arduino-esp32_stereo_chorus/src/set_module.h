#ifndef MODULE_H_
#define MODULE_H_

#include "controller_mod.h"
#include "bsdsp.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~ DSP Class Declarations (bsdsp files) ~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 extern fractionalDelay delay1;
 extern fractionalDelay delay2;
 //extern bool x;
 //extern bool y;
  extern oscillator lfo1;
  extern oscillator lfo2;

//Create a child class derived from controllerModule 
//The controller_module sets up all Pot, Switch, and LED pin, mode, and actions

class controller_module:public controllerModule 
{
  public:
  float depth;
  float freq; 
  float beatFrequency;
  float phaseDiff;
  bool asynch;
  bool stereo;

  void init();
  void onButtonChange(int buttonIndex);
  void onControlChange(int controlIndex);
};

//controller_module myPedal declaration with extern
extern controller_module *myPedal ; 

#endif