#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "freertos/semphr.h"
#include "esp_check.h"

#include <array>
#include "driver/i2s_tdm.h"

#include "peripheral_cfg.h"

#include "Buffers.hpp"

#define BUF_LEN = 3 * 4 * SAMPLE_COUNT;

class AK4619VN_buffers {
    public:

    private:


};