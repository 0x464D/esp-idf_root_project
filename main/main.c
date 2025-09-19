#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#ifdef CONFIG_USE_FMA_LVGL
#include "fma_lvgl_lilygo.h"
#endif

#ifdef CONFIG_USE_FMA_GPIO
#include "fma_gpio.h"
#endif

static const char *TAG = "MAIN";

// ---- app_main ----
void app_main(void)
{
    ESP_LOGI(TAG, "Inicializando...");

    #ifdef CONFIG_USE_FMA_GPIO
        fma_gpio_init();
    #endif

    #ifdef CONFIG_USE_FMA_LVGL
        fma_lvgl_init();
        xTaskCreate(fma_lvgl_task, "fma_lvgl_task", 4096, NULL, 5, NULL);
    #endif

    #if defined(CONFIG_USE_FMA_GPIO) && defined(CONFIG_USE_FMA_LVGL)
        gpio_register_callback_for_pin(GPIO_INPUT_PIN_1, fma_lvgl_gpio_callback_boton1);
        gpio_register_callback_for_pin(GPIO_INPUT_PIN_2, fma_lvgl_gpio_callback_boton2);
        gpio_register_callback_for_pin(GPIO_INPUT_PIN_1_3sec_PRESSED, fma_lvgl_gpio_callback_boton1_3sec);
        gpio_register_callback_for_pin(GPIO_INPUT_PIN_1_5sec_PRESSED, fma_lvgl_gpio_callback_boton1_5sec);

    #endif
}
