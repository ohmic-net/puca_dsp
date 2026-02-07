#include "dsp.h"
#include <math.h>

extern Audio audio;

constexpr float INT16_TO_FLOAT = 1.0f / 32768.0f;

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
    // Temporary buffer for raw mic data
    // 2 channels * DSP_BLOCK_SIZE samples
    static int16_t micBuffer[2 * DSP_BLOCK_SIZE];

    // Read interleaved mic samples: mic0, mic1, mic0, mic1, ...
    audio.readSamples(micBuffer, sizeof(micBuffer));

    for (int i = 0; i < DSP_BLOCK_SIZE; i++) {

        // Convert to float for DSP
        float mic0 = micBuffer[2 * i]     * INT16_TO_FLOAT;
        float mic1 = micBuffer[2 * i + 1] * INT16_TO_FLOAT;

        // Broadside delay-and-sum (zero delay)
        float y = 0.5f * (mic0 + mic1);  // average to prevent clipping

        // Endfire array (uncomment if enabled)
        // int16_t delayed = delayLine[delayIndex];
        // delayLine[delayIndex] = micBuffer[2 * i + 1];
        // delayIndex = (delayIndex + 1) % END_FIRE_DELAY_SAMPLES;
        // float mic1_delayed = delayed * INT16_TO_FLOAT;
        // float y = 0.5f * (x0 + delayed_x1);

        // Other DSP code here

        // Back to int16
        if (y > 1.0f)       y = 1.0f;
        else if (y < -1.0f) y = -1.0f;
        int16_t out = static_cast<int16_t>(y * 32767.0f);

        // Mono output duplicated to L/R
        dspBuffer[2 * i]     = out;
        dspBuffer[2 * i + 1] = out;
    }

    audio.writeSamples(dspBuffer, sizeof(dspBuffer));
}
