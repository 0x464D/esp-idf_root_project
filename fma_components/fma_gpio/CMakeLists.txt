idf_component_register(SRCS
    $<$<BOOL:${CONFIG_USE_FMA_GPIO}>:src/fma_gpio.c>
    INCLUDE_DIRS "." "include"
    REQUIRES esp_driver_gpio
)