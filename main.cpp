#include <stdio.h>
#include <time.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include <hardware/watchdog.h>
#include "board.h"
#include "console.h"
#include "config.h"
#include "i2c_handler.h"

int main() {
    stdio_init_all();
    
    // 設定を初期化
    config_init();
    
    board_init();

    // I2Cスレーブとして初期化
    i2c_slave_setup();

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
