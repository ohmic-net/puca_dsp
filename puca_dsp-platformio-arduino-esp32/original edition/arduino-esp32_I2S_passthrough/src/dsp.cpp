#include "dsp.h"
#include <math.h>

extern Audio audio;

constexpr float INT16_TO_FLOAT = 1.0f / 32768.0f;

// ------------------------------------------------
// Constructor
// ------------------------------------------------
DSP::DSP() = default;

// ------------------------------------------------
// Init
// ------------------------------------------------
void DSP::init()
{
    // Nothing to initialize for passthrough example
}

// ------------------------------------------------
// Audio processing
// ------------------------------------------------
void DSP::processBlock()
{
    // Read stereo I2S input (interleaved)
    audio.readSamples(dspBuffer, sizeof(dspBuffer));

    for (int i = 0; i < DSP_BLOCK_SIZE; i++) {
        // ------------------------------------------------
        // Input: int16 -> float
        // ------------------------------------------------
        float left  = dspBuffer[2 * i]     * INT16_TO_FLOAT;
        float right = dspBuffer[2 * i + 1] * INT16_TO_FLOAT;

        // ------------------------------------------------
        // DSP processing (passthrough)
        // ------------------------------------------------
        float outL = left;
        float outR = right;

        outL = fmaxf(-1.0f, fminf(1.0f, outL));
        outR = fmaxf(-1.0f, fminf(1.0f, outR));

        // ------------------------------------------------
        // Output: float -> int16
        // ------------------------------------------------
        dspBuffer[2 * i]     = static_cast<int16_t>(outL * 32767.0f);
        dspBuffer[2 * i + 1] = static_cast<int16_t>(outR * 32767.0f);
    }

    // Write back to I2S
    audio.writeSamples(dspBuffer, sizeof(dspBuffer));
}








