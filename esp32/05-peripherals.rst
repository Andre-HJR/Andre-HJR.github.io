================================
ESP32 外设驱动
================================

本章介绍 ESP32 常用外设的驱动方法。

UART 串口
=========

.. code-block:: c

   #include "driver/uart.h"
   #include "driver/gpio.h"

   #define UART_NUM   UART_NUM_1
   #define TXD_PIN    GPIO_NUM_17
   #define RXD_PIN    GPIO_NUM_16
   #define BUF_SIZE   1024

   void uart_init(void)
   {
       const uart_config_t uart_config = {
           .baud_rate = 115200,
           .data_bits = UART_DATA_8_BITS,
           .parity    = UART_PARITY_DISABLE,
           .stop_bits = UART_STOP_BITS_1,
           .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
           .source_clk = UART_SCLK_DEFAULT,
       };

       uart_param_config(UART_NUM, &uart_config);
       uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN,
                    UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
       uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
   }

   void uart_send(const char *data)
   {
       uart_write_bytes(UART_NUM, data, strlen(data));
   }

   int uart_recv(char *buf, size_t len)
   {
       return uart_read_bytes(UART_NUM, buf, len, pdMS_TO_TICKS(100));
   }

I2C
====

.. code-block:: c

   #include "driver/i2c.h"

   #define I2C_MASTER_SCL    GPIO_NUM_22
   #define I2C_MASTER_SDA    GPIO_NUM_21
   #define I2C_MASTER_FREQ   100000   // 100 kHz

   void i2c_init(void)
   {
       i2c_config_t conf = {
           .mode = I2C_MODE_MASTER,
           .sda_io_num = I2C_MASTER_SDA,
           .scl_io_num = I2C_MASTER_SCL,
           .sda_pullup_en = GPIO_PULLUP_ENABLE,
           .scl_pullup_en = GPIO_PULLUP_ENABLE,
           .master.clk_speed = I2C_MASTER_FREQ,
       };
       i2c_param_config(I2C_NUM_0, &conf);
       i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
   }

   esp_err_t i2c_write_reg(uint8_t dev_addr,
                           uint8_t reg_addr, uint8_t data)
   {
       uint8_t buf[] = {reg_addr, data};
       return i2c_master_write_to_device(
           I2C_NUM_0, dev_addr, buf, 2, pdMS_TO_TICKS(100));
   }

   uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg_addr)
   {
       uint8_t data;
       i2c_master_write_read_device(
           I2C_NUM_0, dev_addr, &reg_addr, 1,
           &data, 1, pdMS_TO_TICKS(100));
       return data;
   }

SPI
====

.. code-block:: c

   #include "driver/spi_master.h"

   #define SPI_MOSI    GPIO_NUM_23
   #define SPI_MISO    GPIO_NUM_19
   #define SPI_SCLK    GPIO_NUM_18
   #define SPI_CS      GPIO_NUM_5

   spi_device_handle_t spi;

   void spi_init(void)
   {
       spi_bus_config_t buscfg = {
           .mosi_io_num = SPI_MOSI,
           .miso_io_num = SPI_MISO,
           .sclk_io_num = SPI_SCLK,
           .quadwp_io_num = -1,
           .quadhd_io_num = -1,
           .max_transfer_sz = 4096,
       };

       spi_device_interface_config_t devcfg = {
           .clock_speed_hz = 10 * 1000 * 1000,  // 10 MHz
           .mode = 0,
           .spics_io_num = SPI_CS,
           .queue_size = 1,
       };

       spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
       spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
   }

   void spi_transmit(uint8_t *tx, uint8_t *rx, size_t len)
   {
       spi_transaction_t t = {
           .length = len * 8,
           .tx_buffer = tx,
           .rx_buffer = rx,
       };
       spi_device_transmit(spi, &t);
   }

I2S（音频/I2S 数字音频）
==========================

.. code-block:: c

   #include "driver/i2s_std.h"

   void i2s_init(void)
   {
       i2s_chan_handle_t tx_handle;

       i2s_chan_config_t chan_cfg = {
           .id = I2S_NUM_0,
           .role = I2S_ROLE_MASTER,
           .dma_desc_num = 6,
           .dma_frame_num = 240,
       };

       i2s_std_config_t std_cfg = {
           .clk_cfg = {
               .sample_rate_hz = 44100,
               .clk_src = I2S_CLK_SRC_DEFAULT,
               .mclk_multiple = I2S_MCLK_MULTIPLE_256,
           },
           .slot_cfg = {
               .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,
               .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,
               .slot_mode = I2S_SLOT_MODE_MONO,
           },
           .gpio_cfg = {
               .mclk = GPIO_NUM_0,
               .bclk = GPIO_NUM_4,
               .ws = GPIO_NUM_5,
               .dout = GPIO_NUM_18,
               .din = GPIO_NUM_19,
           },
       };

       i2s_new_channel(&chan_cfg, &tx_handle, NULL);
       i2s_channel_init_std_mode(tx_handle, &std_cfg);
       i2s_channel_enable(tx_handle);
   }

ADC
====

.. code-block:: c

   #include "driver/adc.h"
   #include "esp_adc_cal.h"

   #define ADC_CHANNEL  ADC1_CHANNEL_6  // GPIO34
   #define ADC_WIDTH    ADC_WIDTH_BIT_12
   #define ADC_ATTEN    ADC_ATTEN_DB_11  // 0~3.3V

   static esp_adc_cal_characteristics_t adc_chars;

   void adc_init(void)
   {
       adc1_config_width(ADC_WIDTH);
       adc1_config_channel_atten(ADC_CHANNEL, ADC_ATTEN);
       esp_adc_cal_characterize(
           ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH, 3300, &adc_chars);
   }

   uint32_t adc_read_raw(void)
   {
       return adc1_get_raw(ADC_CHANNEL);
   }

   uint32_t adc_read_voltage(void)
   {
       uint32_t raw = adc1_get_raw(ADC_CHANNEL);
       return esp_adc_cal_raw_to_voltage(raw, &adc_chars);
   }

.. caution::

   ADC2 引脚在 WiFi 启用时不可用（WiFi 驱动占用了 ADC2 的硬件资源）。
   使用 ADC 时优先选择 ADC1 通道（GPIO32~39）。
