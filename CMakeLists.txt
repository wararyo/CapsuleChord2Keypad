cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(capsulechord_keypad)

pico_sdk_init()

add_subdirectory(utils)
add_executable(main
    main.cpp
    keyset.cpp
    board.cpp
)

pico_enable_stdio_usb(main 0)
pico_enable_stdio_uart(main 1)

pico_add_extra_outputs(main)

target_link_libraries(main
    pico_stdlib
    pico_i2c_slave
    pico_multicore
    hardware_pwm
    hardware_adc
    hardware_i2c
    pico_bootsel_via_double_reset
)