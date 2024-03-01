cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(NAME PedalBoard)
set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(PedalBoard C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

pico_sdk_init()

include(Libs/PicoLed/PicoLed.cmake)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/pico-ads1115/lib)

add_executable(${NAME}
        main.cpp
        PedalHw/input.c
        PedalHw/iohandler.cpp

        # tiny osc
        libs/tinyosc/tinyosc.c
)

# https://forums.raspberrypi.com/viewtopic.php?p=1958230#p1958230
target_compile_definitions(${NAME} PUBLIC
        PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=1000
)

target_include_directories(${NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
        ${CMAKE_CURRENT_LIST_DIR}/libs/tinyosc
)

target_link_libraries(${NAME}
        pico_cyw43_arch_lwip_threadsafe_background
        pico_stdlib
        pico_unique_id
        pico-ads1115
        PicoLed
        #        tinyusb_device
        #        tinyusb_board
)

pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

pico_add_extra_outputs(${NAME})