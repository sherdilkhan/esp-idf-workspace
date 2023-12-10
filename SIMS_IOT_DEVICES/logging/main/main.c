#include <stdio.h>
#include "esp_log.h"

#define TAG "My Tag"

void app_main(void)
{
        printf("Hello from app_main!\n");
        ESP_LOGW(TAG, "From Main APP");
}
