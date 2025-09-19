idf_component_register(SRCS
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:src/fma_lvgl_lilygo.c>
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:./ui/ui.c>
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:./ui/styles.c>
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:./ui/screens.c>
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:./ui/images.c>
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:./ui/ui_image_link_zelda.c>
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:./ui/ui_image_halo.c>

    INCLUDE_DIRS "." "include"  "./ui"
    REQUIRES lvgl driver esp_lcd
)
