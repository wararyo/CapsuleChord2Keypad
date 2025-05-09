#include <stdio.h>
#include <time.h>
#include <pico/stdlib.h>
#include <pico/i2c_slave.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include "board.h"
#include "console.h"
#include "config.h"

#define I2C_ADDR 0x09

// I2Cコマンド定義
#define CMD_GET_KEY_EVENT 0x01  // キーイベントを取得
#define CMD_SET_LED       0x02  // LEDの明るさ設定
#define CMD_ENTER_DFU     0xF0  // DFU（ファームウェアアップデート）モードに入る

// コマンド処理用のバッファ
volatile uint8_t cmd_buffer[4] = {0};
volatile uint8_t cmd_index = 0;
volatile bool cmd_complete = false;

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
    case I2C_SLAVE_RECEIVE:
        // コマンドやデータの受信
        if (cmd_index < sizeof(cmd_buffer)) {
            cmd_buffer[cmd_index++] = i2c_read_byte_raw(i2c);
            
            // コマンドが完了したかチェック
            if (cmd_buffer[0] == CMD_SET_LED && cmd_index == 3) {
                // LEDコマンドは3バイト (コマンド, キー, 明るさ)
                cmd_complete = true;
            } else if (cmd_buffer[0] == CMD_ENTER_DFU && cmd_index == 1) {
                // DFUコマンドは1バイト (コマンドのみ)
                cmd_complete = true;
            }
        }
        break;
    case I2C_SLAVE_REQUEST:
        // マスターからのリクエスト - キーイベントを返す
        i2c_write_byte_raw(i2c, pop_key_event().toByte());
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
            } else if (cmd_buffer[0] == CMD_ENTER_DFU) {
                printf("Entering firmware update mode...\n");
                // Picoboot3のブートローダーに入る
                watchdog_hw->scratch[0] = 1;
                watchdog_reboot(0, 0, 10);
                while (1) {
                    continue;
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

int main() {
    stdio_init_all();
    
    // 設定を初期化
    config_init();
    
    board_init();

    i2c_init(I2C_INST, 100000);
    i2c_slave_init(I2C_INST, I2C_ADDR, &i2c_slave_handler);

    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_set_pulls(PIN_SDA, true, false);
    gpio_set_pulls(PIN_SCL, true, false);

    // コンソールを初期化
    console_init();

    multicore_launch_core1(led_job);

    while(1) {
        // コンソールを更新
        console_update();

        // ボタンを更新
        button_update();
        
        sleep_ms(17);
    }
}
