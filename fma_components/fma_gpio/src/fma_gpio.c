#include "fma_gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include "esp_log.h"


static const char *TAG = "GPIO";

#define GPIO_INPUT_PIN_1 0
#define GPIO_INPUT_PIN_2 35

// Declaración de la cola
QueueHandle_t gpio_evt_queue = NULL;

// Variables para debounce
TickType_t last_isr_time_pin1 = 0;
TickType_t last_isr_time_pin2 = 0;
TickType_t debounce_delay = 250 / portTICK_PERIOD_MS;  // Tiempo de debounce en milisegundos

// Handler de la interrupción
/*
IRAM se refiere a Instruction RAM
El macro IRAM_ATTR es una directiva de atributo que se utiliza para indicar que una función
debe ser colocada en la IRAM. Esto es útil para funciones que necesitan ser ejecutadas rápidamente,
como los manejadores de interrupciones. Colocar estas funciones en la IRAM evita que se ejecuten
desde la flash (que es más lenta), mejorando así el rendimiento y la latencia.
 */
static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t pin = (uint32_t) arg;

    // Implementación del debounce
    TickType_t current_time = xTaskGetTickCount();

    if (pin == GPIO_INPUT_PIN_1) {
        if ((current_time - last_isr_time_pin1) < debounce_delay) {
            return;
        }
        last_isr_time_pin1 = current_time;
    } else if (pin == GPIO_INPUT_PIN_2) {
        if ((current_time - last_isr_time_pin2) < debounce_delay) {
            return;
        }
        last_isr_time_pin2 = current_time;
    }

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(gpio_evt_queue, &pin, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

#define MAX_GPIO_CALLBACKS 40
static gpio_callback_t gpio_callbacks[MAX_GPIO_CALLBACKS] = {0};  // índice = número de pin

void gpio_register_callback_for_pin(gpio_num_t pin, gpio_callback_t callback) {
    if (pin < MAX_GPIO_CALLBACKS) {
        gpio_callbacks[pin] = callback;
    }
}

// Tarea que maneja la interrupción
/* 
La tarea está esperando a que el semáforo sea liberado.
xSemaphoreTake bloqueará la tarea hasta que el semáforo esté disponible.
El portMAX_DELAY hace que la tarea espere indefinidamente hasta que ocurra una interrupción.

Bajo Consumo de CPU:
Mientras la tarea está bloqueada en xSemaphoreTake, no está utilizando el tiempo de CPU.
El sistema operativo FreeRTOS suspende la tarea y permite que otras tareas usen el procesador.
Cuando el semáforo es liberado por la interrupción, FreeRTOS despierta la tarea.

Eficiencia:
Este patrón es eficiente porque la tarea solo se ejecuta cuando hay trabajo que hacer
(en este caso, manejar una interrupción). No está ocupando tiempo de CPU innecesariamente.
*/
static void interrupt_task(void* arg) {
    uint32_t pin;
    while (1) {
        if (xQueueReceive(gpio_evt_queue, &pin, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Interrupcion de borde de caida detectada en el pin %lu", pin);
            
            // if (pin == GPIO_INPUT_PIN_1) {
            // } else if (pin == GPIO_INPUT_PIN_2) {
            // }
            gpio_callbacks[pin]();
        }
    }
}

void fma_gpio_init() {

    ESP_LOGI(TAG, "Inicializando...");

    // Crear la cola para manejar las interrupciones
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (gpio_evt_queue == NULL) {
        ESP_LOGI(TAG, "No se pudo crear la cola de eventos GPIO");
        return;
    }

    // Configuración de los pines como entrada
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Interrupción en el borde de caída
    io_conf.pin_bit_mask = (1ULL << GPIO_INPUT_PIN_1) | (1ULL << GPIO_INPUT_PIN_2); // Máscara de los pines
    io_conf.mode = GPIO_MODE_INPUT; // Modo de entrada
    io_conf.pull_up_en = 1; // Habilitar pull-up interno
    gpio_config(&io_conf);

    // Instalar el servicio de interrupciones GPIO
    gpio_install_isr_service(0);

    // Añadir el handler de la interrupción para cada pin
    gpio_isr_handler_add(GPIO_INPUT_PIN_1, gpio_isr_handler, (void*) GPIO_INPUT_PIN_1);
    gpio_isr_handler_add(GPIO_INPUT_PIN_2, gpio_isr_handler, (void*) GPIO_INPUT_PIN_2);

    // Crear la tarea para manejar la interrupción
    xTaskCreate(interrupt_task, "interrupt_task", 2048, NULL, 10, NULL);
}
