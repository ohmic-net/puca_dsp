// I2S Configuration for PUCA_DSP development board 

#include "Audio.h"

void Audio::i2s_init(void)
{
    i2s_config_t i2s_config = {
                .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
                .sample_rate = I2S_SAMPLE_RATE,
                .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
                .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
                .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
                .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
                .dma_buf_count = BUF_COUNT,          
                .dma_buf_len = BLOCK_SIZE,          
                .use_apll = true
            };
    
     i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_MCLKPIN,
        .bck_io_num = I2S_BCLK,
        .ws_io_num = I2S_LRC,
        .data_out_num = I2S_DOUT,
        .data_in_num = I2S_DIN                                        
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);                            // install the driver on Core 0
    i2s_set_pin(I2S_NUM_0, &pin_config);                                            // port & pin config 
    REG_WRITE(PIN_CTRL, 0xFFFFFFF0);                                                // 0xFFFFFFF0 for I2S_NUM_0, 0xFFFFFFFF for I2S_NUM_1                                      

    i2s_set_sample_rates(I2S_NUM_0, I2S_SAMPLE_RATE);                             // set sample rate used for I2S TX and RX
}


