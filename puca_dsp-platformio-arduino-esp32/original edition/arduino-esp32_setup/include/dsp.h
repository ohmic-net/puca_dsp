#pragma once

#include <cstdint>
#include <cstddef>
#include "audio.h"

constexpr size_t DSP_BLOCK_SIZE = 128;

class DSP {
public:
    DSP();
    void init();
    void processBlock();

private:
    // Triangle oscillators
    float triangleL();
    float triangleR();

    // Oscillator state
    float phaseL = 0.0f;
    float phaseR = 0.0f;
    float phaseIncL = 0.0f;
    float phaseIncR = 0.0f;

    // Audio buffer
    int16_t dspBuffer[DSP_BLOCK_SIZE * 2]{};
};

