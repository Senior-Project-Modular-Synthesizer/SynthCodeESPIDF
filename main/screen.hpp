#pragma once

#include "lvgl.h"

void initialize_display();
void screen_thingy();
void ili9488_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_buf);