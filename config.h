#pragma once

#include <stdint.h>
#include "keycodes.h"
#include "hardware/flash.h"

// フラッシュメモリの設定
#define FLASH_CONFIG_OFFSET (256 * 1024)  // フラッシュメモリの先頭から256KBの位置を使用
#define CONFIG_MAGIC 0x43434B50           // "CCKP" (Capsule Chord KeyPad) in ASCII

// 設定種別
typedef enum {
    CONFIG_TYPE_BUTTON_THRESHOLDS = 1,
    CONFIG_TYPE_SERIAL_NUMBER = 2,
    // 後で他の設定種別を追加可能
} config_type_t;

// ボタンの感度補正値構造体
typedef struct {
    uint16_t thresholds[0x25];            // 各キーコードに対応する閾値
} button_config_t;

// シリアルナンバー設定
typedef struct {
    char serial[16];                       // シリアルナンバー（最大15文字+終端文字）
} serial_config_t;

// フラッシュメモリに保存される設定構造体
typedef struct {
    uint32_t magic;                       // 構造体が有効か確認するためのマジック番号
    button_config_t buttons;              // ボタン設定
    serial_config_t serial;               // シリアルナンバー設定
    // 将来的に他の設定も追加可能
} device_config_t;

// 設定関連の関数
void config_init();
bool load_config();
bool save_config();
void use_default_config();

// ボタン閾値関連の関数
uint16_t get_button_threshold(keycode_t key);
void set_button_threshold(keycode_t key, uint16_t threshold);

// シリアルナンバー関連の関数
const char* get_serial_number();
bool set_serial_number(const char* serial);
