#ifndef BMP_H
#define BMP_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
//#include "bmp280.h"

/**
 * @brief Task to initialize and read data from the BMP280 sensor.
 * 
 * This task initializes the BMP280 or BME280 sensor and continuously reads
 * temperature, pressure, and optionally humidity (if it's a BME280 sensor).
 * It prints the sensor data every 500ms.
 * 
 * @param pvParameters Parameters to pass to the task (unused in this case).
 */
void bmp280_test(void *pvParameters);

#endif // BMP280_TEST_H
