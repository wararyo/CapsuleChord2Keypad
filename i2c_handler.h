#pragma once

#include <pico/stdlib.h>
#include <hardware/i2c.h>
#include <pico/i2c_slave.h>
#include "board.h"

// I2Cアドレス定義
#define I2C_ADDR 0x09

// I2Cコマンド定義
#define CMD_GET_KEY_EVENT 0x01  // キーイベントを取得
#define CMD_SET_LED       0x02  // LEDの明るさ設定
#define CMD_ENTER_DFU     0xF0  // DFU（ファームウェアアップデート）モードに入る

// I2C通信を初期化する
void i2c_slave_setup();