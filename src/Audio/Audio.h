// I2S Configuration for PUCA DSP development board 

#ifndef __AUDIO_H
#define __AUDIO_H

#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define I2S_SAMPLE_RATE     (32000)   // I2S sample rate 

// Define 12S pins
#define I2S_BCLK       23  // Bit clock 
#define I2S_LRC        25  // Left Right / WS Clock
#define I2S_DOUT       26  
#define I2S_DIN        27
#define I2S_MCLKPIN     0     // MCLK 

class Audio 
{
public:
    void I2S_init(void);
}; 

#endif
