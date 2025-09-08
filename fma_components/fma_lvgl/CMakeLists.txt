idf_component_register(SRCS
    $<$<BOOL:${CONFIG_USE_FMA_LVGL}>:src/fma_lvgl_lilygo.c>
    INCLUDE_DIRS "." "include"
    REQUIRES lvgl
)
