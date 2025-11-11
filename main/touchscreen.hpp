#pragma once

#include "lvgl.h"

void init_touchscreen();

void touchscreen_cb(lv_indev_t *indev, lv_indev_data_t *data);