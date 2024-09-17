#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "led.h"
#include "esp_check.h"
#include "version.h"

#define TAG "main"


void app_main(void) {

    led_initialize();

    led_set_state(LED_STARTUP);

    ESP_LOGI(TAG, "starting SmartAccess (Ullu) device v%d.%d", VERSION_MAJOR, VERSION_MINOR);

}
