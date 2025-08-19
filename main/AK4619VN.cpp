#include "AK4619VN.hpp"

#include "driver/gpio.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "driver/spi_master.h"
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"
#include "AK4619VN_constants.h"
#include "esp_log.h"

#define READ_COMMAND_CODE 0b01000011
#define WRITE_COMMAND_CODE 0b11000011

static const char* TAG = "AK4619VN";

// Debug macros - set to 1 to enable debug output
#define DEBUG_AK4619VN 1

#if DEBUG_AK4619VN
#define DEBUG_LOG(format, ...) ESP_LOGI(TAG, "[DEBUG] " format, ##__VA_ARGS__)
#define DEBUG_CHECKPOINT(msg) ESP_LOGI(TAG, "[CHECKPOINT] %s", msg)
#else
#define DEBUG_LOG(format, ...)
#define DEBUG_CHECKPOINT(msg)
#endif

void init_spi(spi_device_handle_t* spi);
void init_i2s(i2s_chan_handle_t* tx_chan, i2s_chan_handle_t* rx_chan);

AK4619VN::AK4619VN() {
    DEBUG_CHECKPOINT("Starting AK4619VN initialization");
    
    DEBUG_LOG("Configuring PDN GPIO pin");
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << PIN_NUM_CODEC_PDN,
        .mode = GPIO_MODE_OUTPUT,       // Set as output mode
        .intr_type = GPIO_INTR_DISABLE, // Disable interrupts
    };
    gpio_config(&io_conf);
    
    DEBUG_LOG("Power-up sequence: PDN low -> delay -> PDN high -> delay");
    gpio_set_level(PIN_NUM_CODEC_PDN, 0); // Set PDN pin low to power up the codec
    vTaskDelay(2);
    gpio_set_level(PIN_NUM_CODEC_PDN, 1); // Set PDN pin high to enable the codec
    vTaskDelay(11); // Wait for codec to initialize
    DEBUG_CHECKPOINT("Power-up sequence completed");

    DEBUG_LOG("Initializing SPI interface");
    init_spi(&this->spi);

    // Loop through first 16 registers and read from them
    DEBUG_LOG("Reading initial register values");
    for (uint8_t reg = 0; reg < 16; ++reg) {
        uint8_t value = readRegister(reg);
        this->registerCache[reg] = value; // Cache the register value
        ESP_LOGI(TAG, "Register 0x%02X: 0x%02X", reg, value);
    }

    DEBUG_CHECKPOINT("SPI initialization completed");
    
    DEBUG_LOG("Initializing I2S interface");
    init_i2s(&this->tx_chan, &this->rx_chan);
    DEBUG_CHECKPOINT("I2S initialization completed");

    DEBUG_LOG("Testing register reads to verify SPI communication");
    uint8_t reg0 = readRegister(0x00);
    ESP_LOGI(TAG, "Register 0x00: 0x%02X", reg0);
    uint8_t reg12 = readRegister(0x0C);
    ESP_LOGI(TAG, "Register 0x0C: 0x%02X", reg12);
    uint8_t reg13 = readRegister(0x0D);
    ESP_LOGI(TAG, "Register 0x0D: 0x%02X", reg13);
    
    DEBUG_CHECKPOINT("AK4619VN initialization completed successfully");
}

AK4619VN::~AK4619VN() {
    DEBUG_CHECKPOINT("Starting AK4619VN cleanup");
    
    // Cleanup I2S channels
    DEBUG_LOG("Deleting I2S channels");
    i2s_del_channel(this->tx_chan);
    i2s_del_channel(this->rx_chan);

    // Cleanup SPI device
    DEBUG_LOG("Removing SPI device");
    spi_bus_remove_device(this->spi);

    // Power down codec
    DEBUG_LOG("Powering down codec");
    gpio_set_level(PIN_NUM_CODEC_PDN, 0); // Set PDN pin low to power down the codec
    
    DEBUG_CHECKPOINT("AK4619VN cleanup completed");
}

uint8_t AK4619VN::readRegister(uint8_t reg) {
    DEBUG_LOG("Reading register 0x%02X", reg);
    
    // AK4619VN SPI read format: [CMD][ADDR_HIGH][ADDR_LOW] -> [DATA]
    // CMD=0x43, ADDR_HIGH=0x00, ADDR_LOW=register_address
    // Need 4-byte transaction due to SPI shift register behavior
    uint8_t tx_data[4] = {READ_COMMAND_CODE, 0x00, reg, 0x00};
    uint8_t rx_data[4] = {0};
    
    spi_transaction_t trans = {
        .length = 32,        // 4 bytes total transaction
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };
    
    esp_err_t ret = spi_device_transmit(this->spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI read failed for register 0x%02X: %s", reg, esp_err_to_name(ret));
        return 0xFF; // Return error value
    }
    
    // Log the full response to understand the protocol
    DEBUG_LOG("TX: [0x%02X][0x%02X][0x%02X][0x%02X]", 
              tx_data[0], tx_data[1], tx_data[2], tx_data[3]);
    DEBUG_LOG("RX: [0x%02X][0x%02X][0x%02X][0x%02X]", 
              rx_data[0], rx_data[1], rx_data[2], rx_data[3]);
    
    // The register data should be in the last byte due to SPI timing
    uint8_t data = rx_data[3];
    DEBUG_LOG("Register 0x%02X = 0x%02X (from byte 3)", reg, data);
    return data;
}

void AK4619VN::writeRegister(uint8_t reg, uint8_t data) {
    /// TODO: Implement writeRegister
}

// When using SPI interface, release the power-down state of the AK4619 while the CSN pin is “H”. SPI
// interface mode becomes enabled by sending the dummy command mentioned below. To send the
// dummy command, input “0xDE → 0xADDA → 0x7A” to the SI pin after a falling edge of CAD/CSN
// (while it is kept “L”). The data is in MSB first format.
// https://www.akm.com/content/dam/documents/products/audio/audio-codec/ak4619vn/ak4619vn-en-datasheet.pdf#%5B%7B%22num%22%3A523%2C%22gen%22%3A0%7D%2C%7B%22name%22%3A%22XYZ%22%7D%2C54%2C769%2C0%5D
void init_spi(spi_device_handle_t* spi) {
    DEBUG_CHECKPOINT("Starting SPI initialization");
    
    esp_err_t ret;
    spi_device_interface_config_t devcfg = {
        .mode = 0,                 // SPI mode 0
        .clock_speed_hz = 1000000, // 1 MHz
        .spics_io_num = PIN_NUM_CODEC_CS,
        .queue_size = 7,
    };

    DEBUG_LOG("Adding SPI device to bus");
    ret = spi_bus_add_device(SPI_HOST, &devcfg, spi);
    ESP_ERROR_CHECK(ret);

    DEBUG_LOG("Sending SPI dummy command sequence");
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
    
    DEBUG_CHECKPOINT("SPI initialization completed successfully");
}

void init_i2s(i2s_chan_handle_t* tx_chan, i2s_chan_handle_t* rx_chan) {
    DEBUG_CHECKPOINT("Starting I2S initialization");
    
    esp_err_t ret;
    DEBUG_LOG("Creating I2S channels");
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, tx_chan, NULL));
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, rx_chan));

    DEBUG_LOG("Configuring I2S TDM mode");
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
    
    DEBUG_LOG("Enabling I2S channels");
    ret = i2s_channel_enable(*rx_chan);
    ESP_ERROR_CHECK(ret);
    ret = i2s_channel_enable(*tx_chan);
    ESP_ERROR_CHECK(ret);
    
    DEBUG_CHECKPOINT("I2S initialization completed successfully");
}