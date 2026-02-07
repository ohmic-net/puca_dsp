#include "audio.h"
#include "esp_err.h"
#include "driver/i2s.h"

int16_t i2sBuffer[BLOCK_SIZE * 2]; // global i2s buffer

Audio::Audio() {
}

Audio::~Audio() {
    i2s_driver_uninstall(I2S_PORT);
}

void Audio::init() {
    // Configure I2S driver
    i2s_config_t cfg = {};
    cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX);
    cfg.sample_rate = SAMPLE_RATE;
    cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;  // stereo
    cfg.communication_format = I2S_COMM_FORMAT_I2S;
    cfg.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    cfg.dma_buf_count = 4;
    cfg.dma_buf_len = BLOCK_SIZE;
    cfg.use_apll = false;
    cfg.tx_desc_auto_clear = true;

    // I2S pins
    i2s_pin_config_t pins = {};
    pins.bck_io_num = I2S_BCK_PIN;
    pins.ws_io_num  = I2S_WS_PIN;
    pins.data_out_num = I2S_DATA_PIN;
    pins.data_in_num  = I2S_IN_PIN;

    // Install driver
    ESP_ERROR_CHECK(i2s_driver_install(I2S_PORT, &cfg, 0, nullptr));
    ESP_ERROR_CHECK(i2s_set_pin(I2S_PORT, &pins));
    ESP_ERROR_CHECK(i2s_zero_dma_buffer(I2S_PORT));
}

void Audio::writeSamples(const int16_t* data, size_t size) {
    size_t bytesWritten;
    ESP_ERROR_CHECK(i2s_write(I2S_PORT, (const char*)data, size, &bytesWritten, portMAX_DELAY));
}

void Audio::readSamples(int16_t* data, size_t size) {
    size_t bytesRead;
    ESP_ERROR_CHECK(i2s_read(I2S_PORT, (char*)data, size, &bytesRead, portMAX_DELAY));
}


