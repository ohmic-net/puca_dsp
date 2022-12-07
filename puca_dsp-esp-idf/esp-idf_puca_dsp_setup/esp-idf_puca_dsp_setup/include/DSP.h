// Example DSP header file for PUCA DSP development board 

#ifndef __DSP_H
#define __DSP_H

// DSP parameters 
#define WAVE_FREQ_HZ    (200)  // test waveform frequency
#define PI              (3.14159265)
#define SAMPLES_PER_CYCLE (I2S_SAMPLE_RATE/WAVE_FREQ_HZ)

class DSP 
{
public:
    void test_sine_wave(void);
}; 

#endif