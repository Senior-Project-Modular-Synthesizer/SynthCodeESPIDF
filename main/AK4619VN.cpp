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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

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
    update_cache();
    configure_codec();
    DEBUG_LOG("Updating cache after config");
    update_cache();
    // Loop and print out all cached register values
    for (uint8_t i = 0; i < 0x14; ++i) {
        DEBUG_LOG("Register 0x%02X: 0x%02X", i, this->registerCache[i]);
    }

    DEBUG_CHECKPOINT("SPI initialization completed");
    
    DEBUG_LOG("Initializing I2S interface");
    init_i2s(&this->tx_chan, &this->rx_chan);
    DEBUG_CHECKPOINT("I2S initialization completed");
    
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

void AK4619VN::update_cache() {
    for (uint8_t reg = 0; reg <= 0x14; ++reg) {
        uint8_t value = readRegister(reg);
        this->registerCache[reg] = value; // Cache the register value
        //DEBUG_LOG("Register 0x%02X: 0x%02X", reg, value);
        //DEBUG_LOG("In Binary: 0b"BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(value));
    }
}

uint16_t AK4619VN::readRegister(uint8_t reg) {
    // DEBUG_LOG("Reading register 0x%02X", reg);
    
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
        return 0xFFFF; // Return error value
    }
    
    // // Log the full response to understand the protocol
    // DEBUG_LOG("TX: [0x%02X][0x%02X][0x%02X][0x%02X]", 
    //           tx_data[0], tx_data[1], tx_data[2], tx_data[3]);
    // DEBUG_LOG("RX: [0x%02X][0x%02X][0x%02X][0x%02X]", 
    //           rx_data[0], rx_data[1], rx_data[2], rx_data[3]);
    
    // The register data should be in the last byte due to SPI timing
    uint8_t data = rx_data[3];
    // DEBUG_LOG("Register 0x%02X = 0x%02X (from byte 3)", reg, data);
    return data;
}

uint16_t AK4619VN::writeRegister(uint8_t reg, uint8_t data) {
    uint8_t tx_data[4] = {WRITE_COMMAND_CODE, 0x00, reg, data};
    uint8_t rx_data[4] = {0};

    spi_transaction_t trans = {
        .length = 32,
        .tx_buffer = tx_data,
        .rx_buffer = rx_data
    };

    esp_err_t ret = spi_device_transmit(this->spi, &trans);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI write failed for register 0x%02X: %s", reg, esp_err_to_name(ret));
        return 0xFFFF; // Return error value
    }

    // Log the full response to understand the protocol
    // DEBUG_LOG("TX: [0x%02X][0x%02X][0x%02X][0x%02X]",
    //           tx_data[0], tx_data[1], tx_data[2], tx_data[3]);
    // DEBUG_LOG("RX: [0x%02X][0x%02X][0x%02X][0x%02X]",
    //           rx_data[0], rx_data[1], rx_data[2], rx_data[3]);
    return 0;
}

uint16_t AK4619VN::write_setting(uint8_t reg, uint8_t value, uint8_t width, uint8_t pos) {
    uint8_t old_value = readRegister(reg);
    if (old_value == 0xFFFF) return 0xFFFF; // Error reading register
    uint8_t new_value = (old_value & ~(((1 << width) - 1) << pos)) | ((value & ((1 << width) - 1)) << pos);
    return writeRegister(reg, new_value);
}

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
    // i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    // ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, tx_chan, NULL));
    i2s_chan_config_t rx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&rx_chan_cfg, NULL, rx_chan));

    DEBUG_LOG("Sample rate: %d", I2S_SAMPLE_RATE);

    DEBUG_LOG("Configuring I2S TDM mode");
    i2s_tdm_config_t rx_cfg = {
        .clk_cfg = {
            .sample_rate_hz = I2S_SAMPLE_RATE,
            .clk_src = I2S_CLK_SRC_DEFAULT,
            .ext_clk_freq_hz = 0,
            .mclk_multiple = I2S_MCLK_MULTIPLE_512,
            .bclk_div = 8
        },
        .slot_cfg = {
            .data_bit_width = I2S_DATA_BIT_WIDTH_32BIT,
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT,
            .slot_mode = I2S_SLOT_MODE_STEREO,
            .slot_mask = (i2s_tdm_slot_mask_t)(I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
            .ws_width = 32,
            .ws_pol = false,
            .bit_shift = false,
            .left_align = true,
            .big_endian = false,
            .bit_order_lsb = false,
            .skip_mask = false,
            .total_slot = 4
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

    // i2s_tdm_config_t tx_cfg = {
    //     .clk_cfg = I2S_TDM_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
    //     .slot_cfg = {
    //         .data_bit_width = I2S_DATA_BIT_WIDTH_32BIT,
    //         .slot_bit_width = I2S_SLOT_BIT_WIDTH_32BIT,
    //         .slot_mode = I2S_SLOT_MODE_STEREO,
    //         .slot_mask = (i2s_tdm_slot_mask_t)(I2S_TDM_SLOT0 | I2S_TDM_SLOT1 | I2S_TDM_SLOT2 | I2S_TDM_SLOT3),
    //         .ws_width = I2S_TDM_AUTO_WS_WIDTH,
    //         .ws_pol = false,
    //         .bit_shift = false,
    //         .left_align = true,
    //         .big_endian = false,
    //         .bit_order_lsb = false,
    //         .skip_mask = false,
    //         .total_slot = I2S_TDM_AUTO_SLOT_NUM
    //     },
    //     .gpio_cfg = {
    //         .mclk = GPIO_NUM_47,
    //         .bclk = GPIO_NUM_36,
    //         .ws = GPIO_NUM_35,
    //         .dout = PIN_NUM_CODEC_TX,
    //         .din = PIN_NUM_CODEC_TX_DUMMY,
    //         .invert_flags = {
    //             .mclk_inv = false,
    //             .bclk_inv = false,
    //             .ws_inv   = false,
    //         },
    //     }
    // };

    ret = i2s_channel_init_tdm_mode(*rx_chan, &rx_cfg);
    ESP_ERROR_CHECK(ret);
    //ret = i2s_channel_init_tdm_mode(*tx_chan, &tx_cfg);
    //ESP_ERROR_CHECK(ret);
    
    DEBUG_LOG("Enabling I2S channels");    
    DEBUG_CHECKPOINT("I2S initialization completed successfully");
}

void AK4619VN::simple_loop() {
    esp_err_t ret;  
    #define EXAMPLE_BUFF_SIZE 2048
    uint8_t *r_buf = (uint8_t *)calloc(1, EXAMPLE_BUFF_SIZE);
    assert(r_buf); // Check if r_buf allocation success
    size_t r_bytes = 0;

    // Enable i2s
    DEBUG_LOG("Enabling I2S channels");
 
    ret = i2s_channel_enable(rx_chan);
    ESP_ERROR_CHECK(ret);

    //wait 100 ms
    vTaskDelay(pdMS_TO_TICKS(100));
        // Power up adcs and dacs
    write_setting(PMAD2_REG, PMADx_POWER_UP, PMAD2_WIDTH, PMAD2_POS);
    write_setting(PMAD1_REG, PMADx_POWER_UP, PMAD1_WIDTH, PMAD1_POS);
    write_setting(PMDA2_REG, PMDAX_POWER_UP, PMDA2_WIDTH, PMDA2_POS);
    write_setting(PMDA1_REG, PMDAX_POWER_UP, PMDA1_WIDTH, PMDA1_POS);
    write_setting(RSTN_REG, RSTN_NORMAL, RSTN_WIDTH, RSTN_POS);
    vTaskDelay(pdMS_TO_TICKS(100));
    while (1) {
        if (i2s_channel_read(rx_chan, r_buf, EXAMPLE_BUFF_SIZE, &r_bytes, 1000) == ESP_OK) {
            //printf("Read Task: i2s read %d bytes\n", r_bytes);
            // Check for nonzero
            int num_nonzero = 0;
            for (int i = 0; i < r_bytes; i++) {
                if (r_buf[i] != 0) {
                    num_nonzero++;
                }
            }
            if (num_nonzero > 0) {
                printf("Read Task: i2s read %d bytes (%d nonzero)\n", r_bytes, num_nonzero);
                // Print out first 4 ints (32 bits/4 bytes)
                for (int i = 0; i < 16 && i < r_bytes; i += 4) {
                    int new_sample = (r_buf[i] << 24) | (r_buf[i + 1] << 16) | (r_buf[i + 2] << 8) | r_buf[i + 3];
                    printf("Sample %d: %d\n", i / 4, new_sample >> 8);
                }
            }
        } else {
            printf("Read Task: i2s read failed\n");
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void AK4619VN::configure_codec() {
    DEBUG_LOG("Configuring codec");
    uint16_t err = 0x0000;

    // Print reg 0x00

    // Enable TDM
    write_setting(TDM_REG, TDM_MODE_ON, TDM_WIDTH, TDM_POS);
    write_setting(DCF_REG, DCF_TDM_128_I2S, DCF_WIDTH, DCF_POS);
    write_setting(DSL_REG, DSL_32_BIT, DSL_WIDTH, DSL_POS);

    // BICK Edge Setting (POTENTIAL ERROR)
    write_setting(BCKP_REG, BCKP_RISING_EDGE, BCKP_WIDTH, BCKP_POS);

    // Set Slow Mode
    write_setting(SDOPH_REG, SDOPH_SLOW_MODE, SDOPH_WIDTH, SDOPH_POS);

    // Slot length (POTENTIAL ERROR)
    write_setting(SLOT_REG, SLOT_SLOT_LENGTH_BASIS, SLOT_WIDTH, SLOT_POS);

    // Word Lengths
    // (Remember every "packet" is 32 bits but the actual data may not cover it all)
    write_setting(DIDL_REG, DIDL_32_BIT, DIDL_WIDTH, DIDL_POS);
    write_setting(DODL_REG, DODL_24_BIT, DODL_WIDTH, DODL_POS);

    // Master Frequency
    write_setting(FS_REG, FS_512_MCLK, FS_WIDTH, FS_POS);

    // Filter settings (Voice off, short delay on, sharp roll off)
    write_setting(AD2VO_REG, ADXVO_VOICE_FILTER_OFF, AD2VO_WIDTH, AD2VO_POS);
    write_setting(AD1VO_REG, ADXVO_VOICE_FILTER_OFF, AD1VO_WIDTH, AD2VO_POS);

    write_setting(AD2SD_REG, ADXSD_SHORT_DELAY_ON, AD2SD_WIDTH, AD2SD_POS);
    write_setting(AD1SD_REG, ADXSD_SHORT_DELAY_ON, AD1SD_WIDTH, AD1SD_POS);

    write_setting(AD2SL_REG, ADXSL_SHARP_ROLL_OFF, AD2SL_WIDTH, AD2SL_POS);
    write_setting(AD1SL_REG, ADXSL_SHARP_ROLL_OFF, AD1SL_WIDTH, AD1SL_POS);

    // Input Modes
    write_setting(AD1LSEL_REG, ADXXSEL_SINGLE_ENDED1, AD1LSEL_WIDTH, AD1LSEL_POS);
    write_setting(AD1RSEL_REG, ADXXSEL_SINGLE_ENDED1, AD1RSEL_WIDTH, AD1RSEL_POS);
    write_setting(AD2LSEL_REG, ADXXSEL_SINGLE_ENDED1, AD2LSEL_WIDTH, AD2LSEL_POS);
    write_setting(AD2RSEL_REG, ADXXSEL_SINGLE_ENDED1, AD2RSEL_WIDTH, AD2RSEL_POS);

    // ATSPAD ATT
    write_setting(ATSPAD_REG, ATSPAD_ATT_4, ATSPAD_WIDTH, ATSPAD_POS);

    // Soft ADC Mutes off
    write_setting(AD2MUTE_REG, ADXMUTE_DISABLE, AD2MUTE_WIDTH, AD2MUTE_POS);
    write_setting(AD1MUTE_REG, ADXMUTE_DISABLE, AD1MUTE_WIDTH, AD1MUTE_POS);

    // High Pass Filter
    write_setting(AD2HPFN_REG, ADXHPF_ENABLE, AD2HPFN_WIDTH, AD2HPFN_POS);
    write_setting(AD1HPFN_REG, ADXHPF_ENABLE, AD1HPFN_WIDTH, AD1HPFN_POS);

    // DACXSEL (POTENTIAL ERROR) (Idk what to do cuz tdm)

    // De emphasis filter
    write_setting(DEM2_REG, DEMX_OFF, DEM2_WIDTH, DEM2_POS);
    write_setting(DEM1_REG, DEMX_OFF, DEM1_WIDTH, DEM1_POS);

    // DAC Digital Volume Transition Time Setting
    write_setting(ATSPDA_REG, ATSPDA_ATT_4, ATSPDA_WIDTH, ATSPDA_POS);
    
    // Soft DAC Mutes off
    write_setting(DA2MUTE_REG, DAXMUTE_DISABLE, DA2MUTE_WIDTH, DA2MUTE_POS);
    write_setting(DA1MUTE_REG, DAXMUTE_DISABLE, DA1MUTE_WIDTH, DA1MUTE_POS);

    // DAC Filter
    write_setting(DA2SD_REG, DAXSD_SHORT_DELAY_ON, DA2SD_WIDTH, DA2SD_POS);
    write_setting(DA1SD_REG, DAXSD_SHORT_DELAY_ON, DA1SD_WIDTH, DA1SD_POS);

    write_setting(DA2SL_REG, DAXSL_SHARP_ROLL_OFF, DA2SL_WIDTH, DA2SL_POS);
    write_setting(DA1SL_REG, DAXSL_SHARP_ROLL_OFF, DA1SL_WIDTH, DA1SL_POS);



}