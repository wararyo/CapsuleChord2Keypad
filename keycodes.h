#pragma once

#include <stdint.h>

/*** TWI Structure ***/
// State(1bit) | KeyCode(7bit)
// State: 0 is pressed, 1 is released
// Button: see the following

#define PRESSED 0
#define RELEASED 1

/*** KeyCodes ***/
// Group(3bit) | Button(4bit)

// Left Keypad

#define KEY_LEFT_1      0x01
#define KEY_LEFT_2      0x02
#define KEY_LEFT_3      0x03
#define KEY_LEFT_4      0x04
#define KEY_LEFT_5      0x05
#define KEY_LEFT_6      0x06
#define KEY_LEFT_7      0x07
#define KEY_LEFT_8      0x08
#define KEY_LEFT_9      0x09

// Right Keypad

#define KEY_RIGHT_1      0x11
#define KEY_RIGHT_2      0x12
#define KEY_RIGHT_3      0x13
#define KEY_RIGHT_4      0x14
#define KEY_RIGHT_5      0x15
#define KEY_RIGHT_6      0x16
#define KEY_RIGHT_7      0x17
#define KEY_RIGHT_8      0x18
#define KEY_RIGHT_9      0x19

// Other Keys

#define KEY_L           0x21
#define KEY_R           0x22
#define KEY_LT          0x23
#define KEY_RT          0x24

typedef uint8_t keystate_t;
typedef uint8_t keycode_t;

struct KeyEvent {
    keystate_t state: 1;
    keycode_t key: 7;
    KeyEvent() {
        state = 0;
        key = 0;
    }
    KeyEvent(keystate_t s, keycode_t k) {
        state = s;
        key = k;
    }
    uint8_t toByte() {
        return (state & 0b00000001) << 7 | key & 0b01111111;
    }
    bool isValid() {
        return (key & 0b01111111) != 0;
    }
};
