# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(capsulechord_keypad)

pico_sdk_init()

add_subdirectory(utils)
add_executable(main
    main.cpp
    keyset.cpp
    board.cpp
    console.cpp
    config.cpp
)

pico_enable_stdio_usb(main 0)
pico_enable_stdio_uart(main 1)

pico_add_extra_outputs(main)

pico_set_linker_script(main ${CMAKE_SOURCE_DIR}/memmap_picoboot3.ld)

target_link_libraries(main
    pico_stdlib
    pico_i2c_slave
    pico_multicore
    hardware_pwm
    hardware_adc
    hardware_i2c
    hardware_flash
    pico_bootsel_via_double_reset
)