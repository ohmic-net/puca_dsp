#include "dsp.h"
#include <math.h>

extern Audio audio;

//constexpr float INT16_TO_FLOAT = 1.0f / 32768.0f;
//constexpr float INT24_TO_FLOAT = 1.0f / 8388608.0f;
constexpr float INT32_TO_FLOAT = 1.0f / 2147483648.0f;

DSP::DSP() = default;

// ------------------------------------------------
// Init
// ------------------------------------------------
void DSP::init()
{
}

// ------------------------------------------------
// Audio processing
// ------------------------------------------------
void DSP::processBlock()
{
    // Read interleaved mic samples: mic0, mic1, mic0, mic1, ...
    audio.readSamples(micBuffer, sizeof(micBuffer));

    for (int i = 0; i < DSP_BLOCK_SIZE; i++) {
        
        int32_t mic0 = micBuffer[2 * i]; // left channel
        int32_t mic1 = micBuffer[2 * i + 1];  // right channel
        //printf("Mic0: %ld, Mic1: %ld\n", mic0, mic1); //check the raw samples

        // Convert to float
        float mic_0 = micBuffer[2 * i]     * INT32_TO_FLOAT;
        float mic_1 = micBuffer[2 * i + 1] * INT32_TO_FLOAT;
        //printf("Mic_0: %f\n", mic_0); //left channel
        //printf("Mic_1: %f\n", mic_1); //right channel
        //printf("%08lx\n", micBuffer[0]);  // debug, check data format

        // Broadside delay-and-sum (zero delay)
        //float y = 0.5 * (mic_0 + mic_1);  
        //printf("sum: %f\n", y);
   
        // Endfire array (uncomment if enabled)
        int32_t delayed = delayLine[delayIndex];
        delayLine[delayIndex] = micBuffer[2 * i + 1];
        delayIndex = (delayIndex + 1) % END_FIRE_DELAY_SAMPLES;
        float mic1_delayed = delayed * INT32_TO_FLOAT;
        float y = 0.5 * (mic_0 + mic1_delayed);
        printf("sum: %f\n", y);

        if (y > 1.0f)       y = 1.0f;  //clamp
        else if (y < -1.0f) y = -1.0f;
        
        // Write to the dspBuffer / sd card etc
        dspBuffer[2 * i]     = y; // left channel
        dspBuffer[2 * i + 1] = y; // right channel
    }

}
