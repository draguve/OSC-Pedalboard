cmake_minimum_required(VERSION 3.12)

set(NAME PedalBoard)
set(PICO_BOARD pico_w)

include(pico_sdk_import.cmake)

project(PedalBoard C CXX ASM)

pico_sdk_init()


add_executable(${NAME}
        main.c

        # tiny osc
        libs/tinyosc/tinyosc.c
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
#        tinyusb_device
#        tinyusb_board
)

pico_add_extra_outputs(${NAME})