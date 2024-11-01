#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_gap_ble_api.h"
#include "esp_bt.h"              // For Bluetooth controller functions
#include "esp_bt_main.h"          // For Bluedroid functions
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#define TAG "BLE_SCAN"

// Target MAC address to filter
const uint8_t target_mac[] = { 0xA0, 0x6C, 0x65, 0xE2, 0x09, 0x7B };  

// Buffer to store the last received advertisement data
static uint8_t last_adv_data[31];  
static bool first_packet = true;   // Flag to track if it's the first packet from the target device

// GAP event handler
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
                // Check if the detected device's MAC address matches the target MAC address
                if (memcmp(param->scan_rst.bda, target_mac, sizeof(target_mac)) == 0) {
                    ESP_LOGI(TAG, "Target device found: %02x:%02x:%02x:%02x:%02x:%02x",
                             param->scan_rst.bda[0], param->scan_rst.bda[1], param->scan_rst.bda[2],
                             param->scan_rst.bda[3], param->scan_rst.bda[4], param->scan_rst.bda[5]);

                    // Extract the current advertisement data
                    uint8_t *adv_data = param->scan_rst.ble_adv;
                    int adv_data_len = param->scan_rst.adv_data_len;

                    // Log the advertisement data only if it's the first packet or the data has changed
                    if (first_packet || memcmp(adv_data, last_adv_data, adv_data_len) != 0) {
                        ESP_LOGI(TAG, "New or updated advertisement data from target device:");
                        ESP_LOG_BUFFER_HEX("BLE_ADV", adv_data, adv_data_len);

                        // Update the stored last advertisement data
                        memcpy(last_adv_data, adv_data, adv_data_len);
                        first_packet = false;  // Mark that the first packet has been logged
                    }
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

    // Register GAP callback to handle scan results
    ret = esp_ble_gap_register_callback(gap_event_handler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GAP callback registration failed: %s", esp_err_to_name(ret));
        return;
    } else {
        ESP_LOGI(TAG, "GAP callback registered");
    }

    // Start BLE scanning (set scan duration to 0 for continuous scanning)
    ret = esp_ble_gap_start_scanning(0);  // Continuous scanning
    ESP_ERROR_CHECK(ret);
}
