// I2S Passthrough example for PÚCA DSP development board with 4MB Flash and 8MB PSRAM

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <stdlib.h>
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "driver/periph_ctrl.h"
#include <math.h>
#include "WM8978.h"

#define I2S_SAMPLE_RATE     (32000)   // I2S sample rate 
#define I2S_READLEN         256 * 4

// Define 12S pins
#define I2S_BCLK       23  // Bit clock // 33 on TTGO_TAUDIO
#define I2S_LRC        25  // Left Right / WS Clock
#define I2S_DOUT       26  
#define I2S_DIN        27
#define I2S_MCLKPIN     0     // MCLK 

WM8978 wm8978;   // create an instance of WM8978 class for the audio codec 

void I2S_Init() {

    i2s_config_t i2s_config = {
                .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
                .sample_rate = I2S_SAMPLE_RATE,
                .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
                .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
                .dma_buf_count = 4,  
                .dma_buf_len = I2S_READLEN,   
                .use_apll = true
            };
    
     i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_DIN                                        
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);                            // install the driver 
    i2s_set_pin(I2S_NUM_0, &pin_config);                                            // port & pin config 
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);                   // MCLK on GPIO0
    REG_WRITE(PIN_CTRL, 0xFFFFFFF0);

    i2s_set_sample_rates(I2S_NUM_0, I2S_SAMPLE_RATE);                             // set sample rate used for I2S TX and RX
    i2s_set_clk(I2S_NUM_0, I2S_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO); // Set clock & bit width used for I2S RX and TX
  // bit_clock BCK =  sample rate * (number of channels) * bits_per_sample 
}

/*
 * Digital Filtering Code
 * You can implement your own digital filters (e.g. FIR / IIR / biquad / ...) here.
 * Please mind that you need to use separate storage for left / right channels for stereo filtering.
 */
static inline int16_t dummyfilter(int16_t x)
{
	return x;
}


static void i2s_passthrough() {       // function to read the I2S data from the codec and write it to the output 

size_t i2s_bytes_read = 0;
size_t i2s_bytes_written = 0;

float i2s_samples_read[I2S_READLEN / sizeof(float)];
float i2s_samples_write[I2S_READLEN / sizeof(float)];

	while (1) {    // continuously read data over I2S, pass it through the filtering function and write it to the output
		i2s_bytes_read = I2S_READLEN;
		i2s_read(I2S_NUM_0, i2s_samples_read, I2S_READLEN, &i2s_bytes_read, portMAX_DELAY); 

		/* left channel filter */
		for (uint32_t i = 0; i < i2s_bytes_read / 2; i += 2) {
			i2s_samples_write[i] = dummyfilter(i2s_samples_read[i]);
    }
		/* right channel filter */
		for (uint32_t i = 1; i < i2s_bytes_read / 2; i += 2) {
			i2s_samples_write[i] = dummyfilter(i2s_samples_read[i]);
    }
		i2s_write(I2S_NUM_0, i2s_samples_write, i2s_bytes_read, &i2s_bytes_written, 100);

	}
}

void setup() {
  
  Serial.begin(115200);
  I2S_Init();   // I2S config and driver install

  wm8978.init();            // WM8978 codec initialisation
  wm8978.addaCfg(1,1);      // enable the adc and the dac
  wm8978.inputCfg(0,1,0);   // input config, line in enabled
  wm8978.outputCfg(1,0);    // output config
  wm8978.sampleRate(1);     // set the sample rate to match the I2S SAMPLE RATE
  wm8978.spkVolSet(40);     // speaker volume
  wm8978.hpVolSet(40,40);   // headphone volume
  wm8978.i2sCfg(2,0);       // I2S format MSB, 16Bit

  vTaskDelay(2000/portTICK_RATE_MS);  
  i2s_passthrough();  
 
}

void loop() {
//  vTaskDelay(pdMS_TO_TICKS(100));

}