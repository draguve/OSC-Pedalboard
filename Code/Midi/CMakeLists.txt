cmake_minimum_required(VERSION 3.12)

set(NAME PedalBoard)

include(pico_sdk_import.cmake)

project(PedalBoard C CXX ASM)

pico_sdk_init()

# Add your source files
add_executable(${NAME}
        main.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${NAME} pico_stdlib)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${NAME})