#pragma once

#include <cstdint>
#include <cstddef>
#include "audio.h"

constexpr size_t DSP_BLOCK_SIZE = 256;

class DSP {
public:
    DSP();
    void init();
    void processBlock();

private:
    // Audio buffer
    int16_t dspBuffer[DSP_BLOCK_SIZE * 2]{};
};

