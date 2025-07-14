#include "AK4619VN.hpp"

#include "driver/gpio.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "driver/spi_master.h"
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "AK4619VN_constants.h"

AK4619VN::AK4619VN() {
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PIN_NUM_CODEC_PDN,
        .mode = GPIO_MODE_OUTPUT,       // Set as output mode
        .intr_type = GPIO_INTR_DISABLE, // Disable interrupts
    };
    gpio_config(&io_conf);
    gpio_set_level(PIN_NUM_CODEC_PDN, 0); // Set PDN pin low to power up the codec
    vTaskDelay(2);
    gpio_set_level(PIN_NUM_CODEC_PDN, 1); // Set PDN pin high to enable the codec
    vTaskDelay(11); // Wait for codec to initialize

    init_spi(&this->spi);
    init_i2s(&this->tx_chan, &this->rx_chan);
}

void init_spi(spi_device_handle_t* spi) {
    esp_err_t ret;
    spi_device_interface_config_t devcfg = {
        .mode = 0,                 // SPI mode 0
        .clock_speed_hz = 1000000, // 1 MHz
        .spics_io_num = PIN_NUM_CODEC_CS,
        .queue_size = 7
    };

    ret = spi_bus_add_device(SPI_HOST, &devcfg, spi);
    ESP_ERROR_CHECK(ret);

    // Send dummy command to enable SPI interface mode
    // Sequence: 0xDE → 0xADDA → 0x7A (MSB first)
    uint8_t dummy_cmd[4] = {0xDE, 0xAD, 0xDA, 0x7A};
    spi_transaction_t dummy_trans = {
        .length = 32,  // 4 bytes * 8 bits
        .tx_buffer = dummy_cmd,
        .rx_buffer = NULL
    };
    
    ret = spi_device_transmit(*spi, &dummy_trans);
    ESP_ERROR_CHECK(ret);
}

void init_i2s(i2s_chan_handle_t* tx_chan, i2s_chan_handle_t* rx_chan) {
esp_err_t ret;
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, tx_chan, NULL));
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, rx_chan));

    i2s_tdm_config_t rx_cfg = {
        .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_24BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT,
            .slot_mode = I2S_SLOT_MODE_MONO,
            .slot_mask = (i2s_tdm_slot_mask_t)(I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
            .ws_width = I2S_TDM_AUTO_WS_WIDTH,
            .ws_pol = false,
            .bit_shift = false,
            .left_align = true,
            .big_endian = false,
            .bit_order_lsb = false,
            .skip_mask = false,
            .total_slot = I2S_TDM_AUTO_SLOT_NUM
        },
        .gpio_cfg = {
            .mclk = PIN_NUM_CODEC_MCLK,
            .bclk = PIN_NUM_CODEC_BCLK,
            .ws = PIN_NUM_CODEC_WS,
            .dout = PIN_NUM_CODEC_RX_DUMMY,
            .din = PIN_NUM_CODEC_RX,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        }
    };

    i2s_tdm_config_t tx_cfg = {
        .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_32BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT,
            .slot_mode = I2S_SLOT_MODE_MONO,
            .slot_mask = (i2s_tdm_slot_mask_t)(I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
            .ws_width = I2S_TDM_AUTO_WS_WIDTH,
            .ws_pol = false,
            .bit_shift = false,
            .left_align = true,
            .big_endian = false,
            .bit_order_lsb = false,
            .skip_mask = false,
            .total_slot = I2S_TDM_AUTO_SLOT_NUM
        },
        .gpio_cfg = {
            .mclk = PIN_NUM_CODEC_MCLK,
            .bclk = PIN_NUM_CODEC_BCLK,
            .ws = PIN_NUM_CODEC_WS,
            .dout = PIN_NUM_CODEC_TX,
            .din = PIN_NUM_CODEC_TX_DUMMY,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        }
    };

    ret = i2s_channel_init_tdm_mode(*rx_chan, &rx_cfg);
    ESP_ERROR_CHECK(ret);
    ret = i2s_channel_init_tdm_mode(*tx_chan, &tx_cfg);
    ESP_ERROR_CHECK(ret);
}

AK4619VN::~AK4619VN() {
    // Cleanup I2S channels
    i2s_del_channel(this->tx_chan);
    i2s_del_channel(this->rx_chan);

    // Cleanup SPI device
    spi_bus_remove_device(this->spi);

    // Power down codec
    gpio_set_level(PIN_NUM_CODEC_PDN, 0); // Set PDN pin low to power down the codec
}

