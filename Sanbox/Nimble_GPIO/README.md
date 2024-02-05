
# BLE Server with GPIO Control

This repository contains code for a Bluetooth Low Energy (BLE) server implemented on the ESP32 platform using the ESP-IDF framework. The server exposes a custom GATT service allowing communication with a BLE client. Additionally, the server controls a GPIO pin based on commands received from the client.

# Table of Contents
1. Table of Contents
2. Introduction
3. Dependencies
4. Features
5. GPIO Configuration
6. BLE Service Definition
7. Callbacks
8. Usage
9. Task and Threading
10. License

# Introduction
This project sets up an ESP32 as a BLE server that communicates with a BLE client. The server exposes a custom GATT service with characteristics for reading and writing values. The server also controls a GPIO pin based on the commands received from the client.

# Dependencies
ESP-IDF: The ESP32 IoT Development Framework.
NimBLE: NimBLE is a lightweight and efficient Bluetooth Low Energy (BLE) stack.

# Features
BLE server implementation with GATT service.
GPIO control based on commands received from the BLE client.

# GPIO Configuration
The GPIO pin for control is configured in the code using the gpio_init() function. Modify the GPIO_OUTPUT_PIN macro to set the desired GPIO pin.

# BLE Service Definition
The BLE service is defined in the code using the gatt_svcs structure. It includes characteristics for reading and writing values.

# Callbacks
device_read: Callback registered with BLE GATT layer to handle read operations (Server to Client).
device_write: Callback registered with BLE GATT layer to handle write operations (Client to Server).
ble_gap_event: Callback for handling BLE GAP layer events, such as connection and disconnection.

# Usage
Flash the code onto an ESP32 device using the ESP-IDF.
Connect a BLE client to the ESP32 server.
The server responds to read and write requests, controlling the GPIO pin based on the received commands.

# Task and Threading
The code includes a FreeRTOS task (host_task) responsible for running the NimBLE host stack. The task is initiated in the app_main function.

<<<<<<< HEAD
# License
This code is released under the MIT License. Feel free to use, modify, and distribute it as needed. Contributions are welcome.
=======
License
This code is released under the MIT License. Feel free to use, modify, and distribute it as needed. Contributions are welcome.
>>>>>>> 53a65d6f08317e549c7d1a34ba872ec987e1ff11
