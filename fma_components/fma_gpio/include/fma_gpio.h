
#ifndef FMA_GPIO_H_
#define FMA_GPIO_H_

#include "driver/gpio.h"
/**
 * Starts the GPIO IRAM interruption task with semaphore and bit mask.
 */
void fma_gpio_init();


// Pines de entrada configurados para Lilygo T-Display 135 x 240
// Botón 1: GPIO_NUM_0 (BOOT)
// Botón 2: GPIO_NUM_35 (USER)
#define GPIO_INPUT_PIN_1 0
#define GPIO_INPUT_PIN_1_3sec_PRESSED 1
#define GPIO_INPUT_PIN_1_5sec_PRESSED 2

#define GPIO_INPUT_PIN_2 35

// Callback de funciones externas unidas a pines específicos
typedef void (*gpio_callback_t)(void);
void gpio_register_callback_for_pin(gpio_num_t pin, gpio_callback_t callback);

#endif /* FMA_GPIO_H_ */