#pragma once

#include "driver/i2s.h"

// Audio Configuration Defines
#define SAMPLE_RATE    48000  // Defined as an integer
#define BLOCK_SIZE     256    // I2S BLOCK SIZE
#define I2S_BCK_PIN    23
#define I2S_WS_PIN     25
#define I2S_DATA_PIN   26
#define I2S_IN_PIN     27
#define I2S_MCLK_PIN   0     // MCLK
#define I2S_PORT       I2S_NUM_0

class Audio {
public:
 
    Audio();
    ~Audio();

    void init();
    void writeSamples(const int16_t* data, size_t size);
    void readSamples(int16_t* data, size_t size);

private:
    // I2S buffer for audio data
    int16_t* i2sBuffer;
};
