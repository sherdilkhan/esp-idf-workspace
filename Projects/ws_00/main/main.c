#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_check.h"

#include "led.h"
#include "bmp280.h"
#include "version.h"

#define TAG "MAIN"

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif


void app_main(void) {

    led_initialize();

    led_set_state(LED_ERROR);

    ESP_LOGI(TAG, "starting SmartAccess (Ullu) device v%d.%d", VERSION_MAJOR, VERSION_MINOR);
    
    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreatePinnedToCore(bmp280_test, "bmp280_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);
    
}
