// faust sawtooth example for PICO_DSP development board with 4MB Flash & PSRAM Enabled
// DSP task and i2s driver running on core 1

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sawtooth.h"
#include "WM8978.h"

#if !(USING_DEFAULT_ARDUINO_LOOP_STACK_SIZE)
  uint16_t USER_CONFIG_ARDUINO_LOOP_STACK_SIZE = 8192;
#endif

WM8978 wm8978;
float freq = 0; 
float gain = 0;

void setup() {

  Serial.begin(115200);

  wm8978.init();
  wm8978.addaCfg(1,1); 
  wm8978.inputCfg(0,0,0);     
  wm8978.outputCfg(1,0); 
  wm8978.sampleRate(0); 
  wm8978.micGain(30);
  wm8978.auxGain(0);
  wm8978.lineinGain(0);
  wm8978.spkVolSet(50);
  wm8978.hpVolSet(60,60);
  wm8978.i2sCfg(2,0);  //format 12S, length 16bit

  sawtooth* DSP = new sawtooth(48000, 8);
  DSP->start(); 
  freq = 100;
  gain = 0.5;
  DSP->setParamValue("freq", freq); 
  DSP->setParamValue("gain", gain); 

  vTaskDelay(pdMS_TO_TICKS(100));
} 

void loop() {
      vTaskDelay(pdMS_TO_TICKS(10));
}