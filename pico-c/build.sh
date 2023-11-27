#!/bin/bash

# brew install --cask gcc-arm-embedded

# Exit if any command fails
set -e

# Define the directory for the pico-sdk
export PICO_SDK_PATH="$(pwd)/pico-sdk"
export PICO_EXTRAS_PATH="$(pwd)/pico-extras"
export SRC_DIR="$(pwd)/src"

cp "$PICO_SDK_PATH/external/pico_sdk_import.cmake" pico_sdk_import.cmake
cp "$PICO_EXTRAS_PATH/external/pico_extras_import.cmake" pico_extras_import.cmake
cd src
[ -f json.hpp ] || wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
cd ..

# Create a build directory
[ -d build ] || mkdir -p build
rm -rf build/PicoTest*
cd build


# Run CMake to configure the project and generate a Makefile
# cmake -DCMAKE_C_COMPILER:FILEPATH=/opt/homebrew/bin/arm-none-eabi-gcc -DCMAKE_CXX_COMPILER:FILEPATH=/opt/homebrew/bin/arm-none-eabi-g++ ..
cmake -DCMAKE_C_COMPILER:FILEPATH=/opt/homebrew/bin/arm-none-eabi-gcc -DCMAKE_CXX_COMPILER:FILEPATH=/opt/homebrew/bin/arm-none-eabi-g++ ..

# Build the project
make

echo "Build complete!"
