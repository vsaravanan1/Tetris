cmake_minimum_required(VERSION 3.13)

# Initialize the SDK
include(pico_sdk_import.cmake)
set(PICOTOOL_FORCE_FETCH_FROM_GIT 1)

project(tetris)
# Initialize the SDK
pico_sdk_init()
# Add executable target
add_executable(tetris
        main.c
        ssd1306.h
        font5x7.h
)

# Link libraries (standard + hardware peripherals)
target_link_libraries(tetris
        pico_stdlib
        hardware_spi
        hardware_timer
        hardware_gpio
        hardware_rtc

)


# Create UF2 + BIN files for flashing
pico_add_extra_outputs(tetris)