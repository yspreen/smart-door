# What CMake to start at
cmake_minimum_required(VERSION 3.12)
 
# Initialize the Pico SDK
if(NOT PICO_SDK_PATH)
    set(PICO_SDK_PATH "$ENV{PICO_SDK_PATH}")
endif()
if(NOT PICO_BOARD)
    set(PICO_BOARD "pico_w")
endif()
if(NOT PICO_EXTRAS_PATH)
    set(PICO_EXTRAS_PATH "$ENV{PICO_EXTRAS_PATH}")
endif()

# Include the subsidiary .cmake file to get the SDK
include(pico_sdk_import.cmake)
include(pico_extras_import.cmake)
 
# Set the name and version of the project
project(PicoTest C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

# Initalise the SDK
pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        )

file(GLOB SOURCES "src/*.cpp")
add_executable(PicoTest ${SOURCES})

target_include_directories(PicoTest PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}"
    "src"
)

target_link_libraries(PicoTest pico_cyw43_arch_lwip_threadsafe_background pico_stdlib hardware_flash hardware_pwm)

# Enable USB, UART output
pico_enable_stdio_usb(PicoTest 1)
pico_enable_stdio_uart(PicoTest 0)

# Enable extra outputs (SWD?)
pico_add_extra_outputs(PicoTest)
