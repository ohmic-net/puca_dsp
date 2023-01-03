// Example digital signal process for PUCA DSP development board 

#include <math.h>
#include "DSP.h"
#include "Audio.h"

void DSP::test_sine_wave()             // function to generate a test sine wave
{       
  
    float freq = (WAVE_FREQ_HZ);
    unsigned sample_rate = I2S_SAMPLE_RATE;
    size_t buf_size = (SAMPLES_PER_CYCLE*8);   // number of cycles to store in the buffer
    printf("\r\n free mem=%d, written data=%d\n", esp_get_free_heap_size(), buf_size);

    short *samples_sine = new short[buf_size];    // create buffer to store 16bit samples 
    for(int i=0; i<buf_size; ++i) {
        samples_sine[i] = (pow(2,16)/2-1) * sin(PI*(freq/sample_rate)*i);  // 16bit 
        uint value = 0;
        value += samples_sine[i];
        //printf("Samples_sine=%d\n", value);   // comment out to print the samples data, only use one call to printf in the function
    }
    
    //calculating the dsp buffer size; for 32Khz sample rate we create a 200Hz sine wave, every cycle needs 32000/200 = 160 samples 
    //(4-bytes or 8-bytes each sample) depending on bits_per_sample
    //using 4 buffers, we need 40-samples per buffer 
    //if 2-channels, 16-bit each channel, total buffer is 160*4 = 640 bytes
    //if 2-channels, 24/32-bit each channel, total buffer is 160*8 = 1280 bytes
    //adjust dma buf count and dma buf len in Audio.cpp file as needed

  i2s_set_clk(I2S_NUM_0, I2S_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO); // Set clock & bit width used for I2S RX and TX

  // call the write function and pass the data buffer address and data length to it, write the data to the I2S DMA TX buffer
  size_t i2s_bytes_write = 0;
  i2s_write(I2S_NUM_0, samples_sine, buf_size, &i2s_bytes_write, portMAX_DELAY);  

  delete(samples_sine);
}