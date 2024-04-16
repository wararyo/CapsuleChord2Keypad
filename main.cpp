#include <stdio.h>
#include <time.h>
#include <cmath>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "board.h"

int main() {
    stdio_init_all();
    board_init();

    printf("Hello.\n");

    while(1) {
        button_update();
        sleep_ms(33);
    }

}