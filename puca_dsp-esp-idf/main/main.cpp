#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm> //std max/min
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "WM8978.h"
#define I2S_SAMPLE_RATE     (48000) // I2S sample rate 
#define BUFFER_SIZE         32      // Borrowed from Faust boilerplate, 2048 works too.
#define MULT_S32 2147483647 // Max value for int32
#define MULT_S16 32767 // Max value for int16
#define DIV_S16 3.0518509e-5 // 1/MULT+S16
#define clip(sample) std::max(-MULT_S16, std::min(MULT_S16, ((int)(sample * MULT_S16))));



// Sine wave example parameters 
#define WAVE_FREQ_HZ    (200)  // test waveform frequency
#define PI              (3.14159265)
#define SAMPLES_PER_CYCLE (I2S_SAMPLE_RATE/WAVE_FREQ_HZ)

// Initialise I2S
i2s_chan_handle_t tx_handle;
i2s_chan_handle_t rx_handle;

static void init_i2s(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    // Allocate both tx and rx channel at the same time for full-duplex mode.
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_handle, &rx_handle));
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = GPIO_NUM_0,
            .bclk = GPIO_NUM_23,
            .ws = GPIO_NUM_25,
            .dout = GPIO_NUM_26,
            .din = GPIO_NUM_27,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    std_cfg.clk_cfg.mclk_multiple = I2S_MCLK_MULTIPLE_256;
    // Initialise the channels
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_handle, &std_cfg));
}


static void passthrough(void *args) // A function to pass data from ADC to DAC
{
    uint16_t *buf = (uint16_t *)calloc(1, BUFFER_SIZE);
    if (!buf) {
        printf("No memory for read data buffer");
        abort();
    }
    esp_err_t ret = ESP_OK;
    size_t bytes_read = 0;
    size_t bytes_write = 0;
    printf("Passthrough start");
    while (1) {
        /* Read sample data from ADC */
        ret = i2s_channel_read(rx_handle, buf, BUFFER_SIZE, &bytes_read, 1000);
        if (ret != ESP_OK) {
            printf("i2s read failed");
            abort();
        }
        /* Write sample data to DAC */
        ret = i2s_channel_write(tx_handle, buf, BUFFER_SIZE, &bytes_write, 1000);
        if (ret != ESP_OK) {
            printf("i2s write failed");
            abort();
        }
        if (bytes_read != bytes_write) {
            printf("%d bytes read but only %d bytes are written", bytes_read, bytes_write);
        }
    }
    vTaskDelete(NULL);
}

static void sine_wave(void *args)             // function to generate a test sine wave
{
    float freq = (WAVE_FREQ_HZ);
    unsigned sample_rate = I2S_SAMPLE_RATE;
    size_t buf_size = (SAMPLES_PER_CYCLE*8);   // number of cycles to store in the buffer
    short *samples_sine = new short[buf_size];    // create buffer to store 16bit samples 
    for(int i=0; i<buf_size; ++i) {
        samples_sine[i] = (pow(2,16)/2-1) * sin(PI*(freq/sample_rate)*i);  // 16bit 
        uint value = 0;
        value += samples_sine[i];
        //printf("Samples_sine=%d\n", value);   // comment out to print the samples data, only use one call to printf in the function
    }
    size_t w_bytes = 0;
    while (1){
        i2s_channel_write(tx_handle, samples_sine, buf_size,&w_bytes,1000);
    };
    vTaskDelete(NULL);
}

extern "C" {
    void app_main(void);
}

WM8978 wm8978;   // create an instance of WM8978 class for the audio codec 
void app_main() { 
  
    init_i2s();
    ESP_ERROR_CHECK(i2s_channel_enable(tx_handle));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_handle));

    wm8978.init();            // WM8978 codec initialisation
    wm8978.addaCfg(1,1);      // enable the adc and the dac
    wm8978.inputCfg(0,1,0);   // Enable linein
    wm8978.lineinGain(3);
    wm8978.outputCfg(1,0);    // Enable dac out, no bypass.
    wm8978.spkVolSet(0);      // speaker volume not required, set to 0.
    wm8978.hpVolSet(40,40);   // headphone volume
    wm8978.sampleRate(0);     // set sample rate to 48kHz
    wm8978.i2sCfg(2,0);       // I2S format Philips, 16bit

    xTaskCreate(sine_wave, "sine_wave", 4096, NULL, 5, NULL);
    // xTaskCreate(passthrough, "passthrough", 4096, NULL, 5, NULL);
}