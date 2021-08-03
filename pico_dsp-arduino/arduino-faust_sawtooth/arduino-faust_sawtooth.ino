// faust sawtooth example for PICO_DSP development board 
// Choose board ESP32 Dev Module, Flash Size 4MB, Partition scheme default 4MB with spiffs, PSRAM Enabled

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sawtooth.h"
#include "WM8978.h"

WM8978 wm8978;
double freq = 0; 
double gain = 0;


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
  wm8978.hpVolSet(40,40);
  wm8978.i2sCfg(2,0);  //format 12S, length 16bit
  
  sawtooth* DSP = new sawtooth(48000, 8);  //create an instance of sawtooth class with sample rate 48k and block size 8
  DSP->start();
  freq = 200;
  gain = 0.6; 
  DSP->setParamValue("freq", freq);
  DSP->setParamValue("gain", gain); 

  vTaskDelay(pdMS_TO_TICKS(50));
}

void loop() {

 vTaskDelay(pdMS_TO_TICKS(10));

}
