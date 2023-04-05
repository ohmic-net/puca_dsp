
#include "set_settings.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// I2S Configuration / from i2s driver library 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void I2S_init(void)
{
 i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = (i2s_bits_per_sample_t) BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMABUFFERCOUNT, 
    .dma_buf_len = DMABUFFERLENGTH,   
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0 
  };

 i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_BCLK, 
    .ws_io_num = I2S_LRC, 
    .data_out_num = I2S_DOUT, 
    .data_in_num = I2S_DIN
  };

  i2s_driver_install((i2s_port_t)I2S_NUM, &i2s_config, 0, NULL);
  i2s_set_pin((i2s_port_t)I2S_NUM, &pin_config);
   
   // Sets i2s Master Clock on GPIO pin 0 when I2S_NUM is 0
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
  //WRITE_PERI_REG(PIN_CTRL, 0xFFF0);
  REG_WRITE(PIN_CTRL, 0XFFFFFFF0);

  i2s_set_sample_rates((i2s_port_t)I2S_NUM, SAMPLE_RATE);
  i2s_set_clk((i2s_port_t)I2S_NUM, SAMPLE_RATE, BITS_PER_SAMPLE, (i2s_channel_t) CHANNEL_COUNT);

} //end of I2S_init( )