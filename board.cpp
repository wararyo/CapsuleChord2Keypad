#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "board.h"

Queue<KeyEvent> events = Queue<KeyEvent>();
keystate_t pressing[0x25] = { RELEASED }; // 各キーが押されているかどうか

// LEDの明るさを保持するバッファ
led_brightness_t led_brightness[0x25] = { LED_DIM }; // デフォルトは暗い

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
    gpio_pull_up(PIN_BUTTON_L);
    gpio_pull_up(PIN_BUTTON_R);
    gpio_pull_up(PIN_BUTTON_LT);
    gpio_pull_up(PIN_BUTTON_RT);

    gpio_put(PIN_BUTTON_COLUMN_1, 1);
    gpio_put(PIN_BUTTON_COLUMN_2, 1);
    gpio_put(PIN_BUTTON_COLUMN_3, 1);
    gpio_put(PIN_BUTTON_COLUMN_4, 1);
    gpio_put(PIN_BUTTON_COLUMN_5, 1);
    gpio_put(PIN_BUTTON_COLUMN_6, 1);

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

    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_set_pulls(PIN_SDA, true, false);
    gpio_set_pulls(PIN_SCL, true, false);
}

void button_update() {
    // ピンを他のモジュールから変更しない限り、下記の出力はすでにセットされているはず
    // gpio_put(PIN_BUTTON_COLUMN_1, 1);
    // gpio_put(PIN_BUTTON_COLUMN_2, 1);
    // gpio_put(PIN_BUTTON_COLUMN_3, 1);
    // gpio_put(PIN_BUTTON_COLUMN_4, 1);
    // gpio_put(PIN_BUTTON_COLUMN_5, 1);
    // gpio_put(PIN_BUTTON_COLUMN_6, 1);

    // Column 1
    gpio_put(PIN_BUTTON_COLUMN_6, 1);
    gpio_put(PIN_BUTTON_COLUMN_1, 0);
    sleep_ms(2); // ここで一定時間待機しないと、同じ行の他の列のボタンの影響を受けてしまう
    adc_select_input(ADC_BUTTON_ROW_1);
    uint16_t val = adc_read();
    // printf("%d ", val);
    button_handle(val, KEY_LEFT_1);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_LEFT_4);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_LEFT_7);

    // Column 2
    gpio_put(PIN_BUTTON_COLUMN_1, 1);
    gpio_put(PIN_BUTTON_COLUMN_2, 0);
    sleep_ms(2);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_LEFT_2);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_LEFT_5);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_LEFT_8);

    // Column 3
    gpio_put(PIN_BUTTON_COLUMN_2, 1);
    gpio_put(PIN_BUTTON_COLUMN_3, 0);
    sleep_ms(2);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_LEFT_3);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_LEFT_6);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_LEFT_9);

    // Column 4
    gpio_put(PIN_BUTTON_COLUMN_3, 1);
    gpio_put(PIN_BUTTON_COLUMN_4, 0);
    sleep_ms(2);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_RIGHT_1);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_RIGHT_4);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_RIGHT_7);

    // Column 5
    gpio_put(PIN_BUTTON_COLUMN_4, 1);
    gpio_put(PIN_BUTTON_COLUMN_5, 0);
    sleep_ms(2);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_RIGHT_2);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_RIGHT_5);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_RIGHT_8);

    // Column 6
    gpio_put(PIN_BUTTON_COLUMN_5, 1);
    gpio_put(PIN_BUTTON_COLUMN_6, 0);
    sleep_ms(2);
    adc_select_input(ADC_BUTTON_ROW_1);
    button_handle(adc_read(), KEY_RIGHT_3);
    adc_select_input(ADC_BUTTON_ROW_2);
    button_handle(adc_read(), KEY_RIGHT_6);
    adc_select_input(ADC_BUTTON_ROW_3);
    button_handle(adc_read(), KEY_RIGHT_9);

    gpio_put(PIN_BUTTON_COLUMN_6, 1);

    // Other Buttons
    button_handle(gpio_get(PIN_BUTTON_L) ? 4096:0, KEY_L);
    button_handle(gpio_get(PIN_BUTTON_R) ? 4096:0, KEY_R);
    button_handle(gpio_get(PIN_BUTTON_LT) ? 4096:0, KEY_LT);
    button_handle(gpio_get(PIN_BUTTON_RT) ? 4096:0, KEY_RT);
}

void button_handle(uint16_t value, keycode_t key, uint16_t threshold) {
  keystate_t previouslyPressing = pressing[key];
  if(previouslyPressing == RELEASED && value <= threshold) {
    printf("Key pressed: 0x%02x\n", key);
    events.push(KeyEvent(PRESSED, key));
    pressing[key] = PRESSED;
  }
  else if(previouslyPressing == PRESSED && value >= (threshold + 150)) {
    printf("Key released: 0x%02x\n", key);
    events.push(KeyEvent(RELEASED, key));
    pressing[key] = RELEASED;
  }
}

void led_set(keycode_t key, led_brightness_t brightness) {
  if (key >= 0x25) return; // 範囲外のキーは無視
  led_brightness[key] = brightness;
}

void led_job() {
  uint8_t i = 0;
  while(1) {
    // LEDの明るさは4段階で表現する
    bool bright = 0; // 標準より明るい
    bool normal = (i & 0b00000001) != 0; // 標準の明るさ(2サイクルに一回点灯)
    bool dim = (i & 0b00000111) != 0; // 標準より暗い(8サイクルに一回点灯)
    bool off = 1; // 消灯

    gpio_put(PIN_LED_ROW_1, 1);
    gpio_put(PIN_LED_ROW_2, 0);
    gpio_put(PIN_LED_ROW_3, 0);
    gpio_put(PIN_LED_COLUMN_1, getLedValue(KEY_LEFT_1, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_2, getLedValue(KEY_LEFT_2, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_3, getLedValue(KEY_LEFT_3, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_4, getLedValue(KEY_RIGHT_1, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_5, getLedValue(KEY_RIGHT_2, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_6, getLedValue(KEY_RIGHT_3, bright, normal, dim, off));
    sleep_us(300);

    gpio_put(PIN_LED_ROW_1, 0);
    gpio_put(PIN_LED_ROW_2, 1);
    gpio_put(PIN_LED_ROW_3, 0);
    gpio_put(PIN_LED_COLUMN_1, getLedValue(KEY_LEFT_4, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_2, getLedValue(KEY_LEFT_5, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_3, getLedValue(KEY_LEFT_6, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_4, getLedValue(KEY_RIGHT_4, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_5, getLedValue(KEY_RIGHT_5, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_6, getLedValue(KEY_RIGHT_6, bright, normal, dim, off));
    sleep_us(300);

    gpio_put(PIN_LED_ROW_1, 0);
    gpio_put(PIN_LED_ROW_2, 0);
    gpio_put(PIN_LED_ROW_3, 1);
    gpio_put(PIN_LED_COLUMN_1, getLedValue(KEY_LEFT_7, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_2, getLedValue(KEY_LEFT_8, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_3, getLedValue(KEY_LEFT_9, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_4, getLedValue(KEY_RIGHT_7, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_5, getLedValue(KEY_RIGHT_8, bright, normal, dim, off));
    gpio_put(PIN_LED_COLUMN_6, getLedValue(KEY_RIGHT_9, bright, normal, dim, off));
    sleep_us(300);
    
    i++;
  }
}

// LEDの明るさに応じて点灯パターンを返す
inline bool getLedValue(keycode_t key, bool bright, bool on, bool dark, bool off) {
  if (pressing[key] == PRESSED) return bright; // 押されているキーは常時点灯
  switch(led_brightness[key]) {
    case LED_BRIGHT:
      return bright;
    case LED_NORMAL:
      return on;
    case LED_DIM:
      return dark;
    case LED_OFF:
    default:
      return off;
  }
}

KeyEvent pop_key_event() {
  if(events.count() == 0) return KeyEvent();
  return events.pop();
}
