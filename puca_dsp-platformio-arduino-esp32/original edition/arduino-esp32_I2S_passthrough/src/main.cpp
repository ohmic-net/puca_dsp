// I2S Passthrough example for PUCA_DSP development board with 4MB Flash and PSRAM

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdlib.h>
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "driver/periph_ctrl.h"
#include <math.h>
#include "WM8978.h"
#include "audio.h"
#include "dsp.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

WM8978 wm8978;   // create an instance of WM8978 class for the audio codec 
Audio audio;
DSP dsp;

void setup() {
  
  Serial.begin(115200);

  wm8978.init();            // WM8978 codec initialisation
  wm8978.addaCfg(1,1);      // enable the adc and the dac
  wm8978.inputCfg(0,1,0);   // input config, line in enabled
  wm8978.outputCfg(1,0);    // output config
  wm8978.sampleRate(1);     // set the sample rate to match the I2S SAMPLE RATE
  wm8978.spkVolSet(40);     // speaker volume
  wm8978.hpVolSet(40,40);   // headphone volume
  wm8978.i2sCfg(2,0);       // I2S format MSB, 16Bit

  audio.init(); 
  dsp.init(); 
}

void loop() {

vTaskDelay(pdMS_TO_TICKS(10)); 
dsp.processBlock(); 

TIMERG1.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
TIMERG1.wdt_feed=1;
TIMERG1.wdt_wprotect=0;

}