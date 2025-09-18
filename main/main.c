#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_timer.h"
#include "lvgl.h"

#include "ui.h"

// Pines del LILYGO T-Display
#define PIN_NUM_MOSI     19
#define PIN_NUM_CLK      18
#define PIN_NUM_CS       5
#define PIN_NUM_DC       16
#define PIN_NUM_RST      23
#define PIN_NUM_BL       4

// Resolución horizontal
#define LCD_H_RES        240
#define LCD_V_RES        135

#define LCD_HOST         SPI2_HOST
#define LCD_PIXEL_CLOCK_HZ (20 * 1000 * 1000)  // 20MHz
#define LVGL_TICK_PERIOD_MS 2

static const char *TAG = "LILYGO_T_DISPLAY";

// LVGL y panel handles
static lv_display_t *display_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static SemaphoreHandle_t lvgl_mux = NULL;

// ---- LVGL Flush Callback ----
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *color_map)
{
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    esp_err_t ret = esp_lcd_panel_draw_bitmap(panel,
                                               area->x1, area->y1,
                                               area->x2 + 1, area->y2 + 1,
                                               (const void *)color_map);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al hacer flush: %s", esp_err_to_name(ret));
    }

    lv_display_flush_ready(disp);
}

// ---- LVGL Tick Timer ----
static void lv_tick_task(void *arg)
{
    lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

// ---- LCD Init ----
static void lcd_init(void)
{
    // Backlight
    gpio_config_t bl_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BL,
    };
    gpio_config(&bl_cfg);
    gpio_set_level(PIN_NUM_BL, 1);

    // SPI Bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * LCD_V_RES * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // SPI IO Config
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = PIN_NUM_DC,
        .cs_gpio_num = PIN_NUM_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &io_handle));

    // Panel Config
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = PIN_NUM_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Ajustes de orientación y márgenes
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, true));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 40, 52));

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
}

// ---- LVGL Init ----
static void lvgl_init(void)
{
    lv_init();

    lvgl_mux = xSemaphoreCreateMutex();
    assert(lvgl_mux != NULL);

    display_handle = lv_display_create(LCD_H_RES, LCD_V_RES);
    lv_display_set_user_data(display_handle, panel_handle);
    lv_display_set_flush_cb(display_handle, lvgl_flush_cb);

    size_t buffer_pixels = LCD_H_RES * 60;
    void *buf1 = heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_DMA);
    void *buf2 = heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1 && buf2);

    lv_display_set_buffers(display_handle, buf1, buf2, buffer_pixels, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_default(display_handle);

    const esp_timer_create_args_t tick_args = {
        .callback = lv_tick_task,
        .name = "lv_tick"
    };
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK(esp_timer_create(&tick_args, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, LVGL_TICK_PERIOD_MS * 1000));
}

// ---- Crear UI demo ----
static void create_demo_ui(void)
{
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x0000FF), LV_PART_MAIN); // verde

    // Crear un label (texto)
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hola ESP32!");

    // Centrar el texto en la pantalla
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // Crear botón
    lv_obj_t *btn = lv_button_create(scr);
    lv_obj_set_size(btn, 100, 40);                         // Tamaño del botón
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 40);              // Posición en el centro

    // Crear etiqueta dentro del botón
    lv_obj_t *label_btn = lv_label_create(btn);
    lv_label_set_text(label_btn, "Pulsame!");
    lv_obj_center(label_btn);
}

// ---- Tarea principal LVGL ----
static int contador = 0;

static void lvgl_task(void *pv)
{
    // Contador interno que guarda el tiempo del ultimo wake (despertar)
    TickType_t last_wake_time = xTaskGetTickCount();

    while (1) {
        if (xSemaphoreTake(lvgl_mux, portMAX_DELAY)) { // Se toma el mutex para evitar que otra tarea acceda a LVGL
            lv_timer_handler();
            ui_tick(); // función de tick personalizada
            xSemaphoreGive(lvgl_mux); // Se libera el mutex
        }

        // Esperar exactamente 10 ms antes de la siguiente iteración (pdMS_TO_TICKS pasa de milisegundos a ticks)
        // Es importante medir ticks para mantener esta precisión para refrescar la pantalla y saber cuanto tiempo ha transcurrido desde el último wake
        // WARNING: Si se usa vTaskDelay(10) puede que no se espere exactamente 10 ms
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(10)); 

        // Cada 100 iteraciones (100 * 10 ms = 1000 ms = 1 segundo)
        static int ticks = 0;   // Solo se inicializa una vez dentro del segmento .data o .bss NO dentro del stack (pila)
        ticks++;
        if (ticks >= 100) {
            ticks = 0;

            // Cambiar texto con contador
            if (xSemaphoreTake(lvgl_mux, portMAX_DELAY)) { // Se toma el mutex para evitar que otra tarea acceda a LVGL
                // Asegúrate de que el textarea está creado y accesible
                char buf[64];
                snprintf(buf, sizeof(buf), "Contador: %d", contador++);
                lv_textarea_set_text(objects.txt_area_v1, buf);
                xSemaphoreGive(lvgl_mux); // Se libera el mutex
        }

        /* Si la tarea hace algo que no sea LVGL, se puede usar este código para medir el tiempo real transcurrido
           y hacer algo cada segundo real (no cada 100 iteraciones de 10 ms que pueden no ser exactos):
           
            static TickType_t last_time = 0;
            TickType_t now = xTaskGetTickCount();

            if (now - last_time >= pdMS_TO_TICKS(1000)) {
                last_time = now;

                // Ha pasado 1 segundo real
            }

        */
    }
}


// ---- app_main ----
void app_main(void)
{
    ESP_LOGI(TAG, "Inicializando...");

    lcd_init();
    lvgl_init();
    
    //create_demo_ui();
    ui_init(); // Inicializar la UI personalizada

    xTaskCreate(lvgl_task, "lvgl_task", 4096, NULL, 5, NULL);
}
