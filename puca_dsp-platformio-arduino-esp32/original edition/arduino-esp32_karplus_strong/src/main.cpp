// basic karplus strong synthesis example, press the button to trigger the pluck 

#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "WM8978.h"
#include "audio.h"
#include "dsp.h"
#include "ui.h"

WM8978 wm8978;
Audio audio;
DSP dsp;
UI ui;

void setup() {

wm8978.init();
wm8978.addaCfg(1,1); 
wm8978.inputCfg(0,1,0);     
wm8978.outputCfg(1,0); 
wm8978.micGain(45);   // 16 = 0dB (0.75 per dB)
wm8978.auxGain(0);
wm8978.lineinGain(0);
wm8978.spkVolSet(0);
wm8978.hpVolSet(50,50);
wm8978.i2sCfg(2,0);  // format MSB, 16Bit 

audio.init(); 
dsp.init();
ui.init();
}

void loop() {

  while(1) {
        vTaskDelay(10 / portTICK_PERIOD_MS);

        ui.update();

        if (ui.getPluckEvent()) {
            dsp.triggerPluck();
        }

        dsp.processBlock();
    }
}

