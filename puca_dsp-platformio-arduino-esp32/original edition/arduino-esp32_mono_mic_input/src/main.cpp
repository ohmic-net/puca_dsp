// Microphone processing example for PUCA_DSP development board with 4MB Flash & PSRAM Enabled
// Reads a 32bit microphone signal and converts it to an array of floats in the range (-1.0/1.0).  
// Open Serial Plotter in Arduino to visualise the input signal

#include <Arduino.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "driver/i2s.h"
#include "WM8978.h"
#include "Audio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

static WM8978 wm8978;     // create an instance of WM8978 class for the audio codec 
static Audio audio;       // create an instance of the I2S audio driver 
 
TaskHandle_t i2s_read_task;  
SemaphoreHandle_t ySemaphore = NULL; 

void i2s_read (void *arg)   // FreeRTOS task to read the i2s data 
{
  int32_t samples_in[AUDIO_CHANNELS*BLOCK_SIZE];    
  float *data_samples_inL = new float[BLOCK_SIZE];  
  float *data_samples_inR = new float[BLOCK_SIZE];
  size_t bytes_read = 0;

  for (;;) {  
  
  bool read_result = i2s_read(I2S_NUM_0, &samples_in, AUDIO_CHANNELS*BLOCK_SIZE*I2S_BITS_PER_SAMPLE_32BIT/8, &bytes_read, portMAX_DELAY);
  uint32_t samples_read = bytes_read / sizeof(int32_t);

              for (size_t i=0; i <BLOCK_SIZE; i++)               
              {
                if (AUDIO_CHANNELS == 1)               // mono
                { 
                  data_samples_inL[i] = (float)samples_in[i*AUDIO_CHANNELS]*DIV_S32;  
                  //Serial.printf("%ld\n", samples_in[i]);
                  Serial.printf("%.2f\n ", data_samples_inL[i]); 
                } 
                else if (AUDIO_CHANNELS == 2)          // stereo, returns the left and right input signals into 2 float arrays
                {
                  data_samples_inL[i] = (float)samples_in[i*AUDIO_CHANNELS]*DIV_S32;
                  data_samples_inR[i] = (float)samples_in[i*AUDIO_CHANNELS+1]*DIV_S32;   
                  Serial.printf("%.2f\n ", data_samples_inR[i]);       
                }                
              }

      TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
      TIMERG0.wdt_feed=1;
      TIMERG0.wdt_wprotect=0;

      if( ySemaphore != NULL )
      {
      }

    } 
}

void setup() {

Serial.begin(115200); 
audio.i2s_init();         // I2S config and driver install

wm8978.init();            // WM8978 codec initialisation
wm8978.addaCfg(1,1);      // enable the adc and the dac
wm8978.inputCfg(1,0,0);   // input config
wm8978.outputCfg(1,0);    // output config
wm8978.spkVolSet(0);      // speaker volume
wm8978.hpVolSet(50,50);   // headphone volume
wm8978.sampleRate(1);     // set sample rate to match I2S sample rate
wm8978.i2sCfg(2,3);       // I2S format MSB, 32Bit
wm8978.micGain(50);       // set the mic gain 

xTaskCreatePinnedToCore(i2s_read, "i2s_read", 8192, NULL, 20, &i2s_read_task, 0);  

}

void loop() {

}