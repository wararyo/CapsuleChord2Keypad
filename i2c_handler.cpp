#include "i2c_handler.h"
#include <stdio.h>
#include <hardware/gpio.h>
#include <hardware/watchdog.h>

// コマンド処理用のバッファ
static volatile uint8_t cmd_buffer[8] = {0}; // 長めのバッファに変更（閾値設定コマンドは4バイト必要）
static volatile uint8_t cmd_index = 0;
static volatile bool cmd_complete = false;

// 現在処理中のコマンド（リクエストタイプを区別するため）
static volatile uint8_t current_command = 0;

// ボタンのアナログ値を格納する配列
static uint16_t button_values[0x25] = {0};
static uint8_t current_key_index = 0;

// I2C割り込みハンドラー
// 注意: 割り込みハンドラーは短時間で処理する必要があります
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
    case I2C_SLAVE_RECEIVE:
        // コマンドやデータの受信
        if (cmd_index < sizeof(cmd_buffer)) {
            cmd_buffer[cmd_index++] = i2c_read_byte_raw(i2c);
            
            // コマンドが完了したかチェック
            if (cmd_index == 1) {
                // コマンドバイトを読み取った時点で現在のコマンドを設定
                current_command = cmd_buffer[0];
                
                // 単一バイトコマンドの処理
                if (cmd_buffer[0] == CMD_GET_KEY_EVENT || 
                    cmd_buffer[0] == CMD_GET_ALL_KEY_VALUE || 
                    cmd_buffer[0] == CMD_SAVE_CONFIG || 
                    cmd_buffer[0] == CMD_ENTER_DFU) {
                    cmd_complete = true;
                }
            }
            else if (cmd_buffer[0] == CMD_SET_LED && cmd_index == 3) {
                // LEDコマンドは3バイト (コマンド, キー, 明るさ)
                cmd_complete = true;
            } 
            else if (cmd_buffer[0] == CMD_SET_BUTTON_THRESHOLD && cmd_index == 4) {
                // 閾値設定コマンドは4バイト (コマンド, キー, 閾値下位バイト, 閾値上位バイト)
                cmd_complete = true;
            }
        }
        break;
        
    case I2C_SLAVE_REQUEST:
        // マスターからのリクエスト - 現在のコマンドに基づいて応答
        if (current_command == CMD_GET_KEY_EVENT) {
            // キーイベントを返す
            i2c_write_byte_raw(i2c, pop_key_event().toByte());
        }
        else if (current_command == CMD_GET_ALL_KEY_VALUE) {
            // すべてのボタン値のうち1つを返す
            uint16_t value = button_values[current_key_index];
            
            // マスターが読み取りを続けると、次々に値を送信
            if (current_key_index % 2 == 0) {
                // 下位バイトを送信
                i2c_write_byte_raw(i2c, value & 0xFF);
            } else {
                // 上位バイトを送信
                i2c_write_byte_raw(i2c, (value >> 8) & 0xFF);
                
                // 次のキーに進む
                current_key_index = (current_key_index + 1) / 2;
                if (current_key_index >= 0x25) {
                    current_key_index = 0;
                }
            }
            
            // バイトカウンターを更新
            current_key_index++;
        }
        else {
            // デフォルトの挙動としては0を返す
            i2c_write_byte_raw(i2c, 0);
        }
        break;
        
    case I2C_SLAVE_FINISH:
        // I2Cトランザクションの終了
        if (cmd_complete) {
            // コマンド実行
            if (cmd_buffer[0] == CMD_SET_LED) {
                uint8_t key = cmd_buffer[1];
                uint8_t brightness_value = cmd_buffer[2];
                
                // 明るさの値を列挙型に変換して設定
                led_brightness_t brightness;
                switch (brightness_value) {
                    case 0:
                        brightness = LED_BRIGHT;
                        break;
                    case 1:
                        brightness = LED_NORMAL;
                        break;
                    case 2:
                        brightness = LED_DIM;
                        break;
                    case 3:
                    default:
                        brightness = LED_OFF;
                        break;
                }
                
                led_set(key, brightness);
            } 
            else if (cmd_buffer[0] == CMD_ENTER_DFU) {
                printf("Entering firmware update mode...\n");
                // Picoboot3のブートローダーに入る
                watchdog_hw->scratch[0] = 1;
                watchdog_reboot(0, 0, 10);
                while (1) {
                    continue;
                }
            }
            else if (cmd_buffer[0] == CMD_GET_ALL_KEY_VALUE) {
                // すべてのボタンの値を読み取る
                printf("I2C: Reading all button values\n");
                
                // board.cppのread_all_key_values関数を使用してすべてのキー値を読み取る
                read_all_key_values(button_values);
                
                // インデックスをリセット
                current_key_index = 0;
            }
            else if (cmd_buffer[0] == CMD_SET_BUTTON_THRESHOLD) {
                uint8_t key = cmd_buffer[1];
                uint16_t threshold = cmd_buffer[2] | (cmd_buffer[3] << 8);
                
                if (key < 0x25) {
                    // 閾値を設定
                    set_button_threshold((keycode_t)key, threshold);
                    printf("I2C: Set key 0x%02X threshold to %d\n", key, threshold);
                }
            }
            else if (cmd_buffer[0] == CMD_SAVE_CONFIG) {
                // 設定を保存
                printf("I2C: Saving configuration\n");
                if (save_config()) {
                    printf("Configuration saved successfully\n");
                } else {
                    printf("Failed to save configuration\n");
                }
            }
            
            // バッファリセット
            cmd_index = 0;
            cmd_complete = false;
        }
        break;
        
    default:
        break;
    }
}

// I2C通信を初期化する
void i2c_slave_setup() {
    i2c_init(I2C_INST, 100000);
    i2c_slave_init(I2C_INST, I2C_ADDR, &i2c_slave_handler);

    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_set_pulls(PIN_SDA, true, false);
    gpio_set_pulls(PIN_SCL, true, false);
}