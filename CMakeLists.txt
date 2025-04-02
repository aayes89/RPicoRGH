# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Set name of project (as PROJECT_NAME) and C/C++ standards
project(RPicoRGH C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

# Tell CMake where to find the executable source file
add_executable(glitch_project
    src/main.c
    src/glitch.pio.h
)

# Generate the PIO header for glitch.pio
pico_generate_pio_header(glitch_project ${CMAKE_CURRENT_LIST_DIR}/src/glitch.pio)

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(glitch_project
    pico_stdlib
    hardware_gpio
    hardware_pio
)

# Enable USB output, disable UART output
pico_enable_stdio_usb(glitch_project 0)
pico_enable_stdio_uart(glitch_project 1)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(glitch_project)
