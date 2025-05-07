#include <stdio.h>
#include <time.h>
#include <pico/stdlib.h>
#include <pico/i2c_slave.h>
#include <pico/multicore.h>
#include <hardware/gpio.h>
#include "board.h"
#include "console.h"
#include "config.h"

#define I2C_ADDR 0x09

// Our handler is called from the I2C ISR, so it must complete quickly. Blocking calls /
// printing to stdio may interfere with interrupt handling.
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    // KeyEvent keyEvent = KeyEvent();
    switch (event) {
    case I2C_SLAVE_RECEIVE:
        // printf("RECEIVE");
        break;
    case I2C_SLAVE_REQUEST:
        i2c_write_byte_raw(i2c, pop_key_event().toByte());
        // printf(".");
        // i2c_
        // keyEvent = pop_key_event();
        // printf("REQUEST");
        // if(keyEvent.isValid()) i2c_write_byte_raw(i2c, keyEvent.toByte());
        // else i2c_write_byte_raw(i2c, 0x00);
        // if(keyEvent.isValid()) printf("HOGE");
        break;
    case I2C_SLAVE_FINISH:
        // printf("FINISH");
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
