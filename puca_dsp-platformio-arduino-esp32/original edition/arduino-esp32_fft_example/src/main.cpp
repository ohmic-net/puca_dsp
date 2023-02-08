// Fast Fourier Transform example for PUCA_DSP development board Original Edition with 4MB Flash and PSRAM enabled
// Uses a modified version of arduinoFFT library from https://github.com/bertrik/NoiseLevel
// Samples the left (mono) microphone input and returns the FFT energy per octave 

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
#include "arduinoFFT.h"

#define OCTAVES 9
const int fft_samples = BLOCK_SIZE; 

TaskHandle_t i2s_read_to_fft_task; 

static WM8978 wm8978;     // create an instance of WM8978 class for the audio codec 
static Audio audio;       // create an instance of the I2S audio driver 

// FFT data
static float real[fft_samples];
static float imag[fft_samples];
static arduinoFFT fft(real, imag, fft_samples, fft_samples);    // create an instance of the FFT object
static float energy[OCTAVES];

// FFT Functions //
static void integerToFloat(int32_t * integer, float *vReal, float *vImag, uint16_t samples)
{
    for (uint16_t i = 0; i < samples; i++) {
        vReal[i] = (integer[i] >> 16) / 10.0;
        vImag[i] = 0.0;
        //Serial.println(vReal[i]); 
    }
}

// calculates energy from Re and Im parts and places it back in the Re part (Im part is zeroed)
static void calculateEnergy(float *vReal, float *vImag, uint16_t samples)
{
    for (uint16_t i = 0; i < samples; i++) {
        vReal[i] = sq(vReal[i]) + sq(vImag[i]);
        vImag[i] = 0.0;
        //Serial.println(vReal[i]); 
    }
}

// sums up energy in bins per octave
static void sumEnergy(const float *bins, float *energies, int bin_size, int num_octaves)
{
    // skip the first bin
    int bin = bin_size;
    for (int octave = 0; octave < num_octaves; octave++) {
        float sum = 0.0;
        for (int i = 0; i < bin_size; i++) {
            sum += real[bin++];
        }
        energies[octave] = sum;
        bin_size *= 2;
        Serial.println(energies[octave]);   // open Serial Plotter in Arduino IDE to view the frequency domain data
    }
}

void i2s_read_to_fft (void *arg)   // FreeRTOS task to read the i2s data and process the fourier transform 
{
  int32_t samples_in[AUDIO_CHANNELS*BLOCK_SIZE];    
  size_t bytes_read = 0;

  for (;;) {  
  
  bool read_result = i2s_read(I2S_NUM_0, &samples_in, AUDIO_CHANNELS*BLOCK_SIZE*I2S_BITS_PER_SAMPLE_32BIT/8, &bytes_read, portMAX_DELAY);
  uint32_t samples_read = bytes_read / sizeof(int32_t);

  integerToFloat(samples_in, real, imag, fft_samples);  // convert the 32bit input samples

  fft.Windowing(FFT_WIN_TYP_FLT_TOP, FFT_FORWARD);  // apply windowing
  fft.Compute(FFT_FORWARD);                         // compute the FFT

  calculateEnergy(real, imag, fft_samples);     // calculate energy in each bin
  sumEnergy(real, energy, 1, OCTAVES);          // sum up energy in bin for each octave

  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
  TIMERG0.wdt_feed=1;
  TIMERG0.wdt_wprotect=0;
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

xTaskCreatePinnedToCore(i2s_read_to_fft, "i2s_read_to_fft", 8192, NULL, 20, &i2s_read_to_fft_task, 0); 
}

void loop() {
}