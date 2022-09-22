// example microphone endfire array beamforming configuration using Faust DSP 
// for PÚCA DSP development board with 4MB Flash and 8MB PSRAM
// DSP delay time is 6 samples

#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "WM8978.h"
#include "picodsp_mics_endfire_array.h"

WM8978 wm8978;

void setup() {
  
  Serial.begin(115200);

  wm8978.init();
  wm8978.addaCfg(1,1); 
  wm8978.inputCfg(1,0,0);     
  wm8978.outputCfg(1,0); 
  wm8978.sampleRate(2); // Sample Rate 24kHz
  wm8978.micGain(45);   // 16 = 0dB (0.75 per dB)
  wm8978.auxGain(0);
  wm8978.lineinGain(0);
  wm8978.spkVolSet(0);
  wm8978.hpVolSet(50,50);
  wm8978.i2sCfg(2,0);  // format MSB, 16Bit 
  vTaskDelay(500 / portTICK_PERIOD_MS);
    
  int SR = 24000;
  int BS = 8;
  picodsp_mics_endfire_array* DSP = new picodsp_mics_endfire_array(SR, BS);

  DSP->start();
  vTaskDelay(500 / portTICK_PERIOD_MS);
}

void loop() {

  //vTaskDelay(pdMS_TO_TICKS(10));
}