// I2S Configuration for PUCA_DSP development board 

#ifndef __AUDIO_H
#define __AUDIO_H

#include "driver/i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

#define I2S_SAMPLE_RATE     48000   // I2S sample rate   
#define BUF_COUNT           4       // range 2 < 128 
#define BLOCK_SIZE          512    // range 8 < 4092  // at least one full cycle for a periodic signal 

// Define 12S pins
#define I2S_MCLKPIN     0     // MCLK 
#define I2S_BCLK       23  // Bit clock 
#define I2S_LRC        25  // Left Right / WS Clock
#define I2S_DOUT       26  
#define I2S_DIN        27

#define AUDIO_CHANNELS      1
#define MULT_S32            2147483647
#define DIV_S32             4.6566129e-10
#define clip(sample) std::max(-MULT_S32, std::min(MULT_S32, ((int32_t)(sample * MULT_S32))));

class Audio 
{
public:
    Audio() {}
    ~Audio() {}

    void i2s_init(void);

private:

}; 

#endif
