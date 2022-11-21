// Setup sketch for PÚCA_DSP development board
// Generates a 16bit sine wave for testing audio output over I2S with WM8978 codec, and a WiFi access point
// In Tools Menu choose board ESP32 Dev Module, Flash Size 4MB, Partition scheme default 4MB with spiffs, PSRAM Enabled

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "driver/i2s.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_sleep.h"
#include "driver/periph_ctrl.h"
#include <math.h>
#include "WM8978.h"
#include "Network.h"

#define I2S_SAMPLE_RATE     (32000)   // I2S sample rate 

#define WAVE_FREQ_HZ    (200)  // test waveform frequency
#define PI              (3.14159265)

// Define 12S pins
#define I2S_BCLK       23  // Bit clock //
#define I2S_LRC        25  // Left Right / WS Clock
#define I2S_DOUT       26  
#define I2S_DIN        27
#define I2S_MCLKPIN     0     // MCLK 

#define SAMPLES_PER_CYCLE (I2S_SAMPLE_RATE/WAVE_FREQ_HZ)
#define uS_TO_S_FACTOR 1000000 // Conversion factor uS to seconds
#define TIME_TO_SLEEP 10 // time in seconds to sleep for

const char* ssid = "PÚCA_DSP";
//const char* password = "";

WM8978 wm8978;   // create an instance of WM8978 class for the audio codec 
network net;     // create an instance of network class for the access point

void I2S_Init() {

    i2s_config_t i2s_config = {
                .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
                .sample_rate = I2S_SAMPLE_RATE,
                .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
                .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
                .dma_buf_count = 4,  
                .dma_buf_len = 40,   // see below
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
}

    //calculating buffer size; for 32Khz sample rate we create a 200Hz sine wave, every cycle needs 32000/200 = 160 samples 
    //(4-bytes or 8-bytes each sample) depending on bits_per_sample
    //using 4 buffers, we need 40-samples per buffer 
    //if 2-channels, 16-bit each channel, total buffer is 160*4 = 640 bytes
    //if 2-channels, 24/32-bit each channel, total buffer is 160*8 = 1280 bytes


static void sine_wave() {       // function to generate sine wave 
  
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
   
  i2s_set_clk(I2S_NUM_0, I2S_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO); // Set clock & bit width used for I2S RX and TX
  // bit_clock BCK =  sample rate * (number of channels) * bits_per_sample 

  // call the write function and pass the data buffer address and data length to it, write the data to the I2S DMA TX buffer
  size_t i2s_bytes_write = 0;
  i2s_write(I2S_NUM_0, samples_sine, buf_size, &i2s_bytes_write, portMAX_DELAY);  

  delete(samples_sine);
}

void setup() {
  
  Serial.begin(115200);
  SPIFFS.begin(true);
  I2S_Init();   // I2S config and driver install

  wm8978.init();            // WM8978 codec initialisation
  wm8978.addaCfg(1,1);      // enable the adc and the dac
  wm8978.sampleRate(1);     // set the sample rate of the codec 
  wm8978.inputCfg(0,0,0);   // input config
  wm8978.outputCfg(1,0);    // output config
  wm8978.spkVolSet(50);     // speaker volume
  wm8978.hpVolSet(55,55);   // headphone volume
  wm8978.i2sCfg(2,0);  // I2S format MSB, 16Bit

  vTaskDelay(2000/portTICK_RATE_MS);  
  //esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  sine_wave();  // play test sine wave
  
  WiFi.softAP(ssid);  // WiFi mode, STA or AP
  net.mdnsInit(); 
  net.serverInit();
  //vTaskDelay(10000/portTICK_RATE_MS);   
  //wm8978.sleep(1); // after 10 seconds, set the audio codec to standby mode 
  //vTaskDelay(5000/portTICK_RATE_MS); 
  //esp_deep_sleep_start();  // after another 5 seconds, enter deep sleep 
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(100));
}
