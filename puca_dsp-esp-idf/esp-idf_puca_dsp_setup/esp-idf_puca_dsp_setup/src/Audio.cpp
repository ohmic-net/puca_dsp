// I2S Configuration for PUCA DSP development board 

#include "Audio.h"

void Audio::I2S_init(void)
{
    i2s_config_t i2s_config = {
                .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
                .sample_rate = I2S_SAMPLE_RATE,
                .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
                .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
                .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
                .dma_buf_count = 4,         // adjust for DSP 
                .dma_buf_len = 40,          // adjust for DSP 
                .use_apll = true
            };
    
     i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_DIN                                        
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);                            // install the driver on Core 0
    i2s_set_pin(I2S_NUM_0, &pin_config);                                            // port & pin config 
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);                   // MCLK on GPIO0
    REG_WRITE(PIN_CTRL, 0xFFFFFFF0);                                                

    i2s_set_sample_rates(I2S_NUM_0, I2S_SAMPLE_RATE);                             // set sample rate used for I2S TX and RX
}