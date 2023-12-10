#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED_PIN 38

#define TAG "main"

void blink_task(void *pvParameter) {
    while (1) {
        gpio_set_level(LED_PIN, 1); // Turn ON the LED
        ESP_LOGI(TAG, "LED is ON");
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_level(LED_PIN, 0); // Turn OFF the LED
        ESP_LOGI(TAG, "LED is OFF");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);

    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);

    // Initialize logging to monitor pin status
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Monitoring pin %d status on the serial port", LED_PIN);
}
