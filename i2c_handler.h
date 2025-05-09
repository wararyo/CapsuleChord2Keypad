#pragma once

#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include "board.h"
#include "config.h"

// I2Cアドレス定義
#define I2C_ADDR 0x09

// I2Cコマンド定義
#define CMD_GET_KEY_EVENT 0x01       // キーイベントを取得
#define CMD_SET_LED 0x02             // LEDの明るさ設定
#define CMD_GET_ALL_KEY_VALUE 0x03   // すべてのボタンのアナログ値を取得
#define CMD_SET_BUTTON_THRESHOLD 0x04 // ボタンの閾値を設定
#define CMD_SAVE_CONFIG 0x05         // 設定をFlashに保存
#define CMD_ENTER_DFU 0xF0           // DFU（ファームウェアアップデート）モードに入る

// I2C通信を初期化する
void i2c_slave_setup();
