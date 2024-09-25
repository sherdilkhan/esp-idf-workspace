#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_gap_ble_api.h"

#include "esp_bt.h"              // Include this for Bluetooth controller functions
#include "esp_bt_main.h"          // Include this for Bluedroid functions
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"


#define TAG "BLE_SCAN"

const uint8_t target_mac[] = { 0xA0, 0x6C, 0x65, 0xE2, 0x09, 0x7B };


static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
                // Log the incoming device's address
                ESP_LOGI("BLE_SCAN", "Device found: %02x:%02x:%02x:%02x:%02x:%02x",
                         param->scan_rst.bda[0], param->scan_rst.bda[1], param->scan_rst.bda[2],
                         param->scan_rst.bda[3], param->scan_rst.bda[4], param->scan_rst.bda[5]);

                // Check if the found device's MAC address matches the target MAC address
                if (memcmp(param->scan_rst.bda, target_mac, sizeof(target_mac)) == 0) {
                    ESP_LOGI("BLE_SCAN", "Target device found: %02x:%02x:%02x:%02x:%02x:%02x",
                             param->scan_rst.bda[0], param->scan_rst.bda[1], param->scan_rst.bda[2],
                             param->scan_rst.bda[3], param->scan_rst.bda[4], param->scan_rst.bda[5]);
                    
                    // Stop scanning for BLE devices
                    esp_err_t err = esp_ble_gap_stop_scanning();
                    if (err == ESP_OK) {
                        ESP_LOGI("BLE_SCAN", "Scanning stopped successfully.");
                    } else {
                        ESP_LOGE("BLE_SCAN", "Failed to stop scanning, error: %s", esp_err_to_name(err));
                    }

                    // Extract the advertising data
                    uint8_t *adv_data = param->scan_rst.ble_adv;
                    int adv_data_len = param->scan_rst.adv_data_len;
                    
                    // Log the advertising data
                    ESP_LOG_BUFFER_HEX("BLE_ADV", adv_data, adv_data_len);
                }
            }
            break;

        default:
            break;
    }
}




void app_main(void) {
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize the Bluetooth controller and enable BLE
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    ESP_ERROR_CHECK(ret);
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    ESP_ERROR_CHECK(ret);

    // Initialize Bluedroid stack
    ret = esp_bluedroid_init();
    ESP_ERROR_CHECK(ret);
    ret = esp_bluedroid_enable();
    ESP_ERROR_CHECK(ret);

    // Register GAP callback
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GAP callback registration failed: %s", esp_err_to_name(ret));
        return;
    } else {
        ESP_LOGI(TAG, "GAP callback registered");
    }
    
    // Start scanning
    ret = esp_ble_gap_start_scanning(30); // Scan for 30 seconds
    ESP_ERROR_CHECK(ret);

}

