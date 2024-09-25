#include <string.h> // Add this line for memcpy
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "nvs_flash.h"
static const char *TAG = "BLE_BEACON";

static void ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_SCAN_RESULT_EVT:
            if (param->scan_rst.search_evt == ESP_GAP_SEARCH_INQ_RES_EVT) {
                // This event is triggered when a device is found
                ESP_LOGI(TAG, "Device found, RSSI: %d", param->scan_rst.rssi);

                // Extract the UUID from the advertisement data
                uint8_t *adv_data = param->scan_rst.ble_adv;
                uint8_t adv_data_len = param->scan_rst.adv_data_len;

                // Assuming the UUID is included in the advertisement data
                // Parse the advertisement data to find your specific UUID
                // Note: This is a simplified example, adapt as necessary
                if (adv_data_len > 0) {
                    // Check for UUID in adv_data
                    // For example, if the UUID is in the first bytes
                    if (memcmp(adv_data, "1B4E0000-B298-46BD-841A-6D1CB464322B", sizeof("1B4E0000-B298-46BD-841A-6D1CB464322B") - 1) == 0) {
                        ESP_LOGI(TAG, "Found target beacon with UUID: 1B4E0000-B298-46BD-841A-6D1CB464322B");
                    }
                }
            }
            break;

        // Handle other BLE events as necessary

        default:
            break;
    }
}

void app_main() {
	
	
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
    
    // Initialize BLE stack and start scanning
    ret = esp_ble_gap_register_callback(ble_gap_event_handler);
    if (ret) {
        ESP_LOGE(TAG, "Failed to register GAP callback, error code = %x", ret);
        return;
    }

    // Start scanning
    esp_ble_gap_start_scanning(30); // Scan for 30 seconds
}
