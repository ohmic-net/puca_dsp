#pragma once

#include <cstdint>
#include <cstddef>
#include "audio.h"

constexpr size_t DSP_BLOCK_SIZE = 128;
constexpr size_t KS_MAX_DELAY   = 1024;

class DSP {
public:
    DSP();
    void init();
    void processBlock();
    void triggerPluck();

private:
    // Output audio buffer (stereo interleaved)
    int16_t dspBuffer[DSP_BLOCK_SIZE * 2]{};

    // --- Karplus-Strong state ---
    float ksBuffer[KS_MAX_DELAY]{};
    int ksIndex = 0;
    int ksLength = 0;
    float ksDecay = 0.992f;
    float ksVelocity = 1.0f; 

    // Trigger flag 
    volatile bool ksTrigger = false;

    // Internal methods
    void ksSetFrequency(float freq);
    void ksPluck();
    float ksProcess();
};