// Minimal configuration example for PUCA_DSP development board 

#include <stdio.h>
#include "driver/i2s.h"
#include <math.h>
#include "WM8978.h"
#include "audio.h"
#include "dsp.h"

extern "C" {
    void app_main(void);
}

WM8978 wm8978;   // create an instance of WM8978 class for the audio codec 
Audio audio;     // create an instance of the audio class
DSP dsp;         // create and instance of the digital signal process class

void app_main() { 

  wm8978.init();            // WM8978 codec initialisation
  wm8978.addaCfg(1,1);      // enable the adc and the dac
  wm8978.inputCfg(0,0,0);   // input config
  wm8978.outputCfg(1,0);    // output config
  wm8978.spkVolSet(40);     // speaker volume
  wm8978.hpVolSet(40,40);   // headphone volume
  wm8978.sampleRate(1);     // set sample rate to match I2S sample rate
  wm8978.i2sCfg(2,0);       // I2S format MSB, 16Bit

  audio.init();         // I2S config and driver install
  dsp.init(); 
  
  while(1) {
    dsp.processBlock();  
  }
}
