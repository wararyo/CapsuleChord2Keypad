#include <stdio.h>
#include <time.h>
#include <cmath>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

const uint IN0 = 26;
const uint IN1 = 27;
const uint OUT0 = 5;
const uint OUT1 = 6;

int main() {
#ifdef PICO_DEFAULT_LED_PIN
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
#endif

    gpio_init(OUT0);
    gpio_set_dir(OUT0, GPIO_OUT);
    gpio_init(OUT1);
    gpio_set_dir(OUT1, GPIO_OUT);

    gpio_put(OUT0, 1);
    gpio_put(OUT1, 1);

    adc_init();
    adc_gpio_init(IN0);
    // adc_gpio_init(IN1);
    adc_select_input(0);

    uint value11 = 0;
    uint value12 = 0;
    uint value21 = 0;
    uint value22 = 0;

    while(1) {
        gpio_put(OUT0, 0);
        gpio_put(OUT1, 1);
        adc_select_input(0);
        value11 = adc_read();
        adc_select_input(1);
        value12 = adc_read();

        gpio_put(OUT0, 1);
        gpio_put(OUT1, 0);
        adc_select_input(0);
        value21 = adc_read();
        adc_select_input(1);
        value22 = adc_read();

        printf("%d %d %d %d\n", value11, value12, value21, value22);
    }

}