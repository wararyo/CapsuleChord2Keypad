#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "board.h"

Queue<KeyEvent> events = Queue<KeyEvent>();
KeySet pressing = KeySet(); // 現在押されているキー

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

void button_update() {
    gpio_put(PIN_BUTTON_COLUMN_1, 1);
    gpio_put(PIN_BUTTON_COLUMN_2, 1);
    gpio_put(PIN_BUTTON_COLUMN_3, 1);
    gpio_put(PIN_BUTTON_COLUMN_4, 1);
    gpio_put(PIN_BUTTON_COLUMN_5, 1);
    gpio_put(PIN_BUTTON_COLUMN_6, 1);

    // Column 1
    gpio_put(PIN_BUTTON_COLUMN_6, 1);
    gpio_put(PIN_BUTTON_COLUMN_1, 0);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_LEFT_1);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_LEFT_4);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_LEFT_7);

    // Column 2
    gpio_put(PIN_BUTTON_COLUMN_1, 1);
    gpio_put(PIN_BUTTON_COLUMN_2, 0);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_LEFT_2);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_LEFT_5);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_LEFT_8);

    // Column 3
    gpio_put(PIN_BUTTON_COLUMN_2, 1);
    gpio_put(PIN_BUTTON_COLUMN_3, 0);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_LEFT_3);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_LEFT_6);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_LEFT_9);

    // Column 4
    gpio_put(PIN_BUTTON_COLUMN_3, 1);
    gpio_put(PIN_BUTTON_COLUMN_4, 0);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_RIGHT_1);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_RIGHT_4);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_RIGHT_7);

    // Column 5
    gpio_put(PIN_BUTTON_COLUMN_4, 1);
    gpio_put(PIN_BUTTON_COLUMN_5, 0);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_RIGHT_2);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_RIGHT_5);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_RIGHT_8);

    // Column 6
    gpio_put(PIN_BUTTON_COLUMN_5, 1);
    gpio_put(PIN_BUTTON_COLUMN_6, 0);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_RIGHT_3);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_RIGHT_6);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_RIGHT_9);
}

void button_handle(uint16_t value, keycode_t key) {
  keystate_t state = (value < 2000) ? PRESSED : RELEASED;
  KeyEvent event {state, key};
  switch(state){
    case PRESSED:
      if(pressing.Add(key)) {
        printf("Key pressed: 0x%02x\n", key);
        events.push(event);
      }
    break;
    case RELEASED:
      if(pressing.Remove(key)) {
        printf("Key released: 0x%02x\n", key);
        events.push(event);
      }
    break;
  }
}
