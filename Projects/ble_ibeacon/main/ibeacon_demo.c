#include <stdio.h>
#include <string.h>
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "nvs_flash.h"
#include "esp_log.h"

static const char *TAG = "iBeacon";

// Define iBeacon UUID, Major, Minor, and TX Power
#define BEACON_UUID {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xAA, 0xFE, 0x00, 0x00} // Custom iBeacon UUID
#define BEACON_MAJOR 0x00 // Major value (custom, can be set to any value)
#define BEACON_MINOR 0x01 // Minor value (custom, can be set to any value)
#define TX_POWER     0xC5 // Transmit power calibration (signal strength at 1 meter)

// Advertising parameters to control how often and what type of advertising is done
static esp_ble_adv_params_t adv_params = {
    .adv_int_min        = 0x20, // Minimum advertisement interval (in 0.625ms units) e.g 0x20 -> 32 in Decimal So 32 * 0.625 = 20mSec
    .adv_int_max        = 0x40, // Maximum advertisement interval (in 0.625ms units) e.g 0x40 -> 64 in Decimal So 34 * 0.625 = 40mSec
    .adv_type           = ADV_TYPE_IND, // Connectable advertising (needed to include device name) or ADV_TYPE_NONCONN_IND
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC, // Public address type
    .channel_map        = ADV_CHNL_ALL, // Advertise on all three BLE advertising channels 37,38,39
    .adv_filter_policy  = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY, // Allow scan and connect requests from any device
};

// Structure to define the iBeacon advertisement packet format
typedef struct {
    uint8_t flags[3];          // Advertising flags (e.g., general discoverable mode)
    uint8_t length;            // Length of manufacturer-specific data
    uint8_t type;              // Type of manufacturer-specific data (0xFF for iBeacon)
    uint8_t company_id[2];     // Apple company identifier (0x004C for iBeacon)
    uint8_t beacon_type[2];    // iBeacon type (0x1502)
    uint8_t uuid[16];          // The unique 128-bit UUID for this iBeacon
    uint8_t major[2];          // Major value (two bytes)
    uint8_t minor[2];          // Minor value (two bytes)
    uint8_t tx_power;          // Calibrated transmit power (signal strength at 1 meter)
} __attribute__((packed)) iBeacon_packet_t; // Ensure the structure is packed with no extra padding bytes

// Function to initialize the iBeacon and set up BLE
void beacon_init(void) {
    // Initialize the Bluedroid stack
    esp_err_t ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid initialization failed: %s", esp_err_to_name(ret));
        return;
    }
    // Enable Bluedroid to start using BLE
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "Bluedroid enable failed: %s", esp_err_to_name(ret));
        return;
    }

    // Define the iBeacon packet structure and set the iBeacon data
    iBeacon_packet_t ibeacon_packet = {
        .flags          = {0x02, 0x01, 0x06}, // Advertising flags: General discoverable mode, BR/EDR not supported
        .length         = 0x1A,               // Length of the manufacturer-specific data here it is 26 byte total
        .type           = 0xFF,               // Manufacturer-specific data type (0xFF for iBeacon)
        .company_id     = {0x4C, 0x00},       // Apple company ID (0x004C, required for iBeacon)
        .beacon_type    = {0x02, 0x15},       // iBeacon type identifier (0x0215)
        .uuid           = BEACON_UUID,        // Custom 128-bit UUID
        .major          = {BEACON_MAJOR >> 8, BEACON_MAJOR & 0xFF}, // Major number (high byte, low byte)
        .minor          = {BEACON_MINOR >> 8, BEACON_MINOR & 0xFF}, // Minor number (high byte, low byte)
        .tx_power       = TX_POWER            // Transmit power (calibrated at 1 meter)
    };

    // Set the device name (e.g., "iBeacon") to make the device name visible in scans
    const char *device_name = "iBeaconSDKN";
    esp_err_t name_ret = esp_ble_gap_set_device_name(device_name); // Set the device name
    if (name_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set device name: %s", esp_err_to_name(name_ret));
    }

    // Set the raw advertisement data (iBeacon packet) for broadcasting
    esp_err_t adv_ret = esp_ble_gap_config_adv_data_raw((uint8_t *)&ibeacon_packet, sizeof(iBeacon_packet_t));
    if (adv_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure advertisement data: %s", esp_err_to_name(adv_ret));
    }

    // Start advertising using the defined advertising parameters
    esp_ble_gap_start_advertising(&adv_params);
    ESP_LOGI(TAG, "iBeacon advertising started with device name 'iBeacon'");
}

// Main application entry point
void app_main(void) {
    esp_err_t ret;

    // Initialize NVS (Non-Volatile Storage) to use Bluetooth
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Release Classic Bluetooth memory if not used (we are only using BLE)
    ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller release classic bt memory failed: %s", esp_err_to_name(ret));
    }

    // Bluetooth controller initialization with default configuration
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller initialization failed: %s", esp_err_to_name(ret));
        return;
    }

    // Enable the Bluetooth controller in BLE mode
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "Bluetooth controller enable BLE mode failed: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize the iBeacon and start advertising
    beacon_init();
}
