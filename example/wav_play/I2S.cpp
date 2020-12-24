#include "I2S.h"

void I2S_Init(int DIN, int DOUT, int LRC, int BCLK)
{
  uint8_t m_i2s_num = I2S_NUM_0; // i2s port number
  i2s_config_t i2s_config = {
      //From ESP32 I2S Audio
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = DEFAULT_BPS,
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
      .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = 1024,
      .use_apll = 1,
      .tx_desc_auto_clear = true,
      .fixed_mclk = I2S_PIN_NO_CHANGE};

  Serial.println("using I2S_MODE");
  i2s_pin_config_t pin_config;
  pin_config.bck_io_num = BCLK;
  pin_config.ws_io_num = LRC;

  pin_config.data_out_num = DOUT;
  pin_config.data_in_num = DIN;

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, DEFAULT_BPS, I2S_CHANNEL_STEREO);
}

int I2S_Read(char *data, int numData)
{
  return i2s_read_bytes(I2S_NUM_0, (char *)data, numData, portMAX_DELAY);
}

void I2S_Write(char *data, int numData)
{
  i2s_write_bytes(I2S_NUM_0, (const char *)data, numData, portMAX_DELAY);
}
