#pragma once

#include "keycodes.h"
#include "keyset.h"
#include "utils/queue.h"
#include "hardware/i2c.h"

// LEDの明るさを表す列挙型
typedef enum {
    LED_BRIGHT = 0,  // 明るい - 常に点灯
    LED_NORMAL = 1,  // 通常 - 2サイクルに1回点灯
    LED_DIM = 2,     // 暗い - 8サイクルに1回点灯
    LED_OFF = 3      // 消灯 - 常に消灯
} led_brightness_t;

#define PIN_BUTTON_ROW_1 28 // 間違えて縦方向を逆に設計してしまった…のでプログラム側を合わせる
#define PIN_BUTTON_ROW_2 27
#define PIN_BUTTON_ROW_3 26
#define PIN_BUTTON_COLUMN_1 5
#define PIN_BUTTON_COLUMN_2 6
#define PIN_BUTTON_COLUMN_3 7
#define PIN_BUTTON_COLUMN_4 8
#define PIN_BUTTON_COLUMN_5 9
#define PIN_BUTTON_COLUMN_6 10

#define PIN_LED_ROW_1 11
#define PIN_LED_ROW_2 12
#define PIN_LED_ROW_3 13
#define PIN_LED_COLUMN_1 14 // 間違えて横方向を逆に設計してしまった…のでプログラム側を合わせる
#define PIN_LED_COLUMN_2 15
#define PIN_LED_COLUMN_3 16
#define PIN_LED_COLUMN_4 17
#define PIN_LED_COLUMN_5 18
#define PIN_LED_COLUMN_6 19

#define PIN_BUTTON_L 20
#define PIN_BUTTON_R 21
#define PIN_BUTTON_LT 22
#define PIN_BUTTON_RT 23

#define PIN_TX 0
#define PIN_RX 1
#define PIN_SDA 2
#define PIN_SCL 3

#define ADC_BUTTON_ROW_1 2
#define ADC_BUTTON_ROW_2 1
#define ADC_BUTTON_ROW_3 0

#define I2C_INST i2c1

void board_init();

void button_update();

void button_handle(uint16_t value, keycode_t key, uint16_t threshold = 3700);

// 指定したキーのLEDの明るさを設定する
void led_set(keycode_t key, led_brightness_t brightness);

// LEDの明るさに応じて点灯パターンを返す
bool getLedValue(keycode_t key, bool bright, bool on, bool dark, bool off);

void led_job();

KeyEvent pop_key_event();
