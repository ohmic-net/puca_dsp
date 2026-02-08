#include "dsp.h"
#include <math.h>

extern Audio audio;

DSP::DSP() = default;

// ------------------------------------------------
// Triangle wave generators
// ------------------------------------------------
inline float DSP::triangleL()
{
    phaseL += phaseIncL;
    if (phaseL >= 1.0f)
        phaseL -= 1.0f;

    return (phaseL < 0.5f)
        ? (4.0f * phaseL - 1.0f)
        : (3.0f - 4.0f * phaseL);
}

inline float DSP::triangleR()
{
    phaseR += phaseIncR;
    if (phaseR >= 1.0f)
        phaseR -= 1.0f;

    return (phaseR < 0.5f)
        ? (4.0f * phaseR - 1.0f)
        : (3.0f - 4.0f * phaseR);
}

// ------------------------------------------------
// Init
// ------------------------------------------------
void DSP::init()
{
    phaseL = 0.0f;
    phaseR = 0.0f;

    phaseIncL = 400.0f / float(SAMPLE_RATE); // Left: 400 Hz
    phaseIncR = 200.0f / float(SAMPLE_RATE); // Right: 200 Hz
}

// ------------------------------------------------
// Audio processing
// ------------------------------------------------
void DSP::processBlock()
{
    for (int i = 0; i < DSP_BLOCK_SIZE; i++) {
        float left  = triangleL();
        float right = triangleR();

        dspBuffer[2 * i]     = (int16_t)(left  * 32767.0f);
        dspBuffer[2 * i + 1] = (int16_t)(right * 32767.0f);
    }

    audio.writeSamples(dspBuffer, sizeof(dspBuffer));
}







