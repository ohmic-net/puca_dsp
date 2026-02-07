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
    // Output audio buffer (stereo interleaved)
    int16_t dspBuffer[DSP_BLOCK_SIZE * 2]{};

    // Input microphone buffer (2 mics, interleaved)
    int16_t micBuffer[DSP_BLOCK_SIZE * 2]{};

    // ------------------------------------------------
    // Endfire beamforming 
    // ------------------------------------------------
    // Uncomment to enable endfire steering
    // static constexpr int END_FIRE_DELAY_SAMPLES = 2;

    // Simple integer delay line (uncomment if enabled)
    // int16_t delayLine[END_FIRE_DELAY_SAMPLES]{};
    // size_t delayIndex = 0;

    // Choosing END_FIRE_DELAY_SAMPLES
    // Use d = n x v/fs
    // where d is the distance between the microphones, n is the number of samples of DSP delay, v is the velocity or speed of sound in air (343m/sec), and fs is the sampling rate or sampling frequency.
    // On PUCA DSP, the distance d between the microphones is calculated to match a delay time of two samples for a sampling rate of 48 kHz, or a delay time of one sample for a sample rate of 24 kHz:
    // d = n x v/fs, when fs is 48 kHz and n=2, = 14.29 mm d = n x v/fs, when fs is 24 kHz and n=1, = 14.29 mm
};