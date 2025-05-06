#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "config.h"

// 設定データの保存場所
static device_config_t device_config;

// フラッシュメモリのアドレスを計算
static const uint8_t *flash_target_contents = (const uint8_t *) (XIP_BASE + FLASH_CONFIG_OFFSET);

// 設定の初期化
void config_init() {
    if (!load_config()) {
        // 読み込みに失敗したらデフォルト値を使用
        use_default_config();
        // デフォルト値を保存
        save_config();
    }
}

// フラッシュメモリから設定を読み込む
bool load_config() {
    // フラッシュメモリから設定を読み込み
    memcpy(&device_config, flash_target_contents, sizeof(device_config_t));
    
    // マジックナンバーをチェックして有効な設定かどうか確認
    if (device_config.magic == CONFIG_MAGIC) {
        printf("Config loaded from flash\n");
        return true;
    }
    
    printf("No valid config found in flash\n");
    return false;
}

// デフォルト設定を使用
void use_default_config() {
    printf("Using default config\n");
    
    // マジックナンバーを設定
    device_config.magic = CONFIG_MAGIC;
    
    // ボタン設定: すべてのボタンにデフォルトの閾値を設定
    for (int i = 0; i < 0x25; i++) {
        device_config.buttons.thresholds[i] = 3700; // デフォルト値
    }

    // シリアルナンバーのデフォルト値を設定
    strncpy(device_config.serial.serial, "CC2P0000", sizeof(device_config.serial.serial) - 1);
    device_config.serial.serial[sizeof(device_config.serial.serial) - 1] = '\0'; // 必ず終端文字を入れる
}

// 設定をフラッシュメモリに保存
bool save_config() {
    printf("Saving config to flash\n");
    
    // interrupt を無効化
    uint32_t ints = save_and_disable_interrupts();
    
    // フラッシュメモリのセクタを消去
    flash_range_erase(FLASH_CONFIG_OFFSET, FLASH_SECTOR_SIZE);
    
    // 設定をフラッシュメモリに書き込み
    flash_range_program(FLASH_CONFIG_OFFSET, (const uint8_t*) &device_config, sizeof(device_config_t));
    
    // interrupt を再度有効化
    restore_interrupts(ints);
    
    return true;
}

// キーコードに対応する閾値を取得
uint16_t get_button_threshold(keycode_t key) {
    if (key < 0x25) {
        return device_config.buttons.thresholds[key];
    }
    return 3700; // 範囲外のキーコードの場合はデフォルト値を返す
}

// キーコードに対応する閾値を設定
void set_button_threshold(keycode_t key, uint16_t threshold) {
    if (key < 0x25) {
        device_config.buttons.thresholds[key] = threshold;
    }
}

// シリアルナンバーを取得
const char* get_serial_number() {
    return device_config.serial.serial;
}

// シリアルナンバーを設定
bool set_serial_number(const char* serial) {
    if (serial == NULL || strlen(serial) >= sizeof(device_config.serial.serial)) {
        return false;
    }
    
    strncpy(device_config.serial.serial, serial, sizeof(device_config.serial.serial) - 1);
    device_config.serial.serial[sizeof(device_config.serial.serial) - 1] = '\0'; // 必ず終端文字を入れる
    return true;
}
