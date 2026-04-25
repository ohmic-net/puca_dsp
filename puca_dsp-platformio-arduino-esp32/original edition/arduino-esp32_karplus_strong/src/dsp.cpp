#include "dsp.h"
#include <math.h>

extern Audio audio;

constexpr float INT16_TO_FLOAT = 1.0f / 32768.0f;

DSP::DSP() = default;

void DSP::init()
{
    ksSetFrequency(220.0f); // A2
    ksPluck();
}

// ------------------------------------------------
// External trigger (call on button edge)
// ------------------------------------------------
void DSP::triggerPluck()
{
     float r = (float)rand() / RAND_MAX;  // [0,1]
    
    // Square to bias toward lower values, then scale to [0.3,1.0] for velocity
    ksVelocity = 0.3f + (r * r) * 0.7f;

    ksTrigger = true;
}

// ------------------------------------------------
// Karplus-Strong setup
// ------------------------------------------------
void DSP::ksSetFrequency(float freq)
{
    ksLength = SAMPLE_RATE / freq;

    if (ksLength < 2) ksLength = 2;
    if (ksLength > KS_MAX_DELAY) ksLength = KS_MAX_DELAY;

    ksIndex = 0;
}

void DSP::ksPluck()
{
float last = 0.0f;

for (int i = 0; i < ksLength; i++) {

    float white = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;

    // low-pass shaping
    last = 0.5f * last + 0.3f * white;

    // exponential decay envelope
    float t = (float)i / (float)ksLength;
    //float env = expf(-5.0f * t);

    float attack = 0.2f; // fraction of ksLength for attack
    float env = (i < attack * ksLength) ?
            (float)i / (attack * ksLength) :   // linear ramp up
            expf(-5.0f * (t - attack));       // decay after attack

    ksBuffer[i] = last * env * ksVelocity;;
    }
}

float DSP::ksProcess()
{
    int next = ksIndex + 1;
    if (next >= ksLength) next = 0;

    float y = ksBuffer[ksIndex];

    // Averaging + simple damping filter
    float avg = 0.5f * (ksBuffer[ksIndex] + ksBuffer[next]);
    avg = 0.7f * avg + 0.3f * ksBuffer[ksIndex];

    ksBuffer[ksIndex] = ksDecay * avg;
    ksIndex = next;

    return y;
}

// ------------------------------------------------
// Audio processing
// ------------------------------------------------
void DSP::processBlock()
{
    // Handle trigger once per block 
    if (ksTrigger) {
        ksPluck();
        ksTrigger = false;
    }

    for (int i = 0; i < DSP_BLOCK_SIZE; i++) {

        // Karplus-Strong synthesis
        float ksSample = ksProcess();

        float y = ksSample;              

        // Clip
        if (y > 1.0f)       y = 1.0f;
        else if (y < -1.0f) y = -1.0f;

        int16_t out = (int16_t)(y * 32767.0f);

        dspBuffer[2 * i]     = out;
        dspBuffer[2 * i + 1] = out;
    }

    audio.writeSamples(dspBuffer, sizeof(dspBuffer));
}
