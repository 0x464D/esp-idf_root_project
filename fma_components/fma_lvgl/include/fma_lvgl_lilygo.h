#ifndef FMA_LVGL_LILYGO_
#define FMA_LVGL_LILYGO_

#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "ui.h"
#include "lvgl.h"

void fma_lvgl_init();

// Solamente se declara static si va a ser usada dentro del mismo archivo .c
void fma_lvgl_task(void *pv);

#endif /* FMA_LVGL_LILYGO_ */
