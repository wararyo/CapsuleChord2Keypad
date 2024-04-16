#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "board.h"

void board_init() {
    adc_init();

    adc_gpio_init(PIN_BUTTON_ROW_1);
    adc_gpio_init(PIN_BUTTON_ROW_2);
    adc_gpio_init(PIN_BUTTON_ROW_3);
    gpio_init(PIN_BUTTON_COLUMN_1);
    gpio_init(PIN_BUTTON_COLUMN_2);
    gpio_init(PIN_BUTTON_COLUMN_3);
    gpio_init(PIN_BUTTON_COLUMN_4);
    gpio_init(PIN_BUTTON_COLUMN_5);
    gpio_init(PIN_BUTTON_COLUMN_6);
    gpio_init(PIN_LED_ROW_1);
    gpio_init(PIN_LED_ROW_2);
    gpio_init(PIN_LED_ROW_3);
    gpio_init(PIN_LED_COLUMN_1);
    gpio_init(PIN_LED_COLUMN_2);
    gpio_init(PIN_LED_COLUMN_3);
    gpio_init(PIN_LED_COLUMN_4);
    gpio_init(PIN_LED_COLUMN_5);
    gpio_init(PIN_LED_COLUMN_6);
    gpio_init(PIN_BUTTON_L);
    gpio_init(PIN_BUTTON_R);
    gpio_init(PIN_BUTTON_LT);
    gpio_init(PIN_BUTTON_RT);
    // gpio_init(PIN_TX);
    // gpio_init(PIN_RX);
    gpio_init(PIN_SDA);
    gpio_init(PIN_SCL);

    gpio_set_dir(PIN_BUTTON_COLUMN_1, GPIO_OUT);
    gpio_set_dir(PIN_BUTTON_COLUMN_2, GPIO_OUT);
    gpio_set_dir(PIN_BUTTON_COLUMN_3, GPIO_OUT);
    gpio_set_dir(PIN_BUTTON_COLUMN_4, GPIO_OUT);
    gpio_set_dir(PIN_BUTTON_COLUMN_5, GPIO_OUT);
    gpio_set_dir(PIN_BUTTON_COLUMN_6, GPIO_OUT);
    gpio_set_dir(PIN_LED_ROW_1, GPIO_OUT);
    gpio_set_dir(PIN_LED_ROW_2, GPIO_OUT);
    gpio_set_dir(PIN_LED_ROW_3, GPIO_OUT);
    gpio_set_dir(PIN_LED_COLUMN_1, GPIO_OUT);
    gpio_set_dir(PIN_LED_COLUMN_2, GPIO_OUT);
    gpio_set_dir(PIN_LED_COLUMN_3, GPIO_OUT);
    gpio_set_dir(PIN_LED_COLUMN_4, GPIO_OUT);
    gpio_set_dir(PIN_LED_COLUMN_5, GPIO_OUT);
    gpio_set_dir(PIN_LED_COLUMN_6, GPIO_OUT);
    gpio_set_dir(PIN_BUTTON_L, GPIO_IN);
    gpio_set_dir(PIN_BUTTON_R, GPIO_IN);
    gpio_set_dir(PIN_BUTTON_LT, GPIO_IN);
    gpio_set_dir(PIN_BUTTON_RT, GPIO_IN);

    // LED全消灯
    gpio_put(PIN_LED_ROW_1, 0);
    gpio_put(PIN_LED_ROW_2, 0);
    gpio_put(PIN_LED_ROW_3, 0);
    gpio_put(PIN_LED_COLUMN_1, 0);
    gpio_put(PIN_LED_COLUMN_2, 0);
    gpio_put(PIN_LED_COLUMN_3, 0);
    gpio_put(PIN_LED_COLUMN_4, 0);
    gpio_put(PIN_LED_COLUMN_5, 0);
    gpio_put(PIN_LED_COLUMN_6, 0);

    i2c_init(i2c1, 100000);
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_set_pulls(PIN_SDA, true, false);
    gpio_set_pulls(PIN_SCL, true, false);
}

