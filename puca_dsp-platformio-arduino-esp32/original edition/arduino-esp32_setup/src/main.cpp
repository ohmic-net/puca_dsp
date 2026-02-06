// Minimal configuration example for PÚCA_DSP development board

#include <Arduino.h>
#include <stdio.h>
#include "driver/i2s.h"
#include <math.h>
#include "WM8978.h"
#include "audio.h"
#include "dsp.h"
#include "Network.h"

WM8978 wm8978;   // create an instance of WM8978 class for the audio codec 
Audio audio;     // create an instance of the I2S audio driver 
DSP dsp;         // create an instance of the digital signal process
network net; 

const char* ssid = "PÚCA_DSP";   // WiFi Access Point
//const char* password = "";

void setup() {

    Serial.begin(115200); 
    SPIFFS.begin(true);

    audio.init();         // I2S config and driver install

    wm8978.init();            // WM8978 codec initialisation
    wm8978.addaCfg(1,1);      // enable the adc and the dac
    wm8978.inputCfg(0,0,0);   // input config
    wm8978.outputCfg(1,0);    // output config
    wm8978.spkVolSet(40);     // speaker volume
    wm8978.hpVolSet(40,40);   // headphone volume
    wm8978.sampleRate(1);     // set sample rate to match I2S sample rate
    wm8978.i2sCfg(2,0);       // I2S format MSB, 16Bit

    dsp.init();

    WiFi.softAP(ssid);  // WiFi mode, STA or AP
    net.mdnsInit(); 
    net.serverInit();
}

void loop() {

    vTaskDelay(pdMS_TO_TICKS(10));
    dsp.processBlock(); 
}
