idf_component_register(SRCS "main.c" "./ui/ui.c" "./ui/styles.c" "./ui/screens.c" "./ui/images.c"
                    INCLUDE_DIRS "" "./ui"
                    REQUIRES lvgl driver esp_lcd)