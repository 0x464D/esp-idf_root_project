
#ifndef FMA_GPIO_H_
#define FMA_GPIO_H_

#include "driver/gpio.h"
/**
 * Starts the GPIO IRAM interruption task with semaphore and bit mask.
 */
void fma_gpio_init();

// Callback de funciones externas unidas a pines específicos
typedef void (*gpio_callback_t)(void);
void gpio_register_callback_for_pin(gpio_num_t pin, gpio_callback_t callback);

#endif /* FMA_GPIO_H_ */