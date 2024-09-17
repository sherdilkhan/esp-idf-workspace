#ifndef LED_H
#define LED_H

typedef enum {
    LED_STARTUP,
    LED_OK,
    LED_BLE_CONNECTED,
    LED_RF_ACTIVE,
    LED_ERROR
} led_state_t;

/* Function prototypes */
void led_initialize(void);
void led_set_state(led_state_t led_state);

#endif /* LED_H */
