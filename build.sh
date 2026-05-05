#!/bin/bash

echo "Building C to Java Transpiler..."

cd "$(dirname "$0")"

if [ -d build ]; then
    echo "Cleaning previous build..."
    rm -rf build
fi

echo "Creating build directory..."
mkdir build
cd build

echo "Configuring with CMake..."
cmake ..
if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

echo "Building project..."
cmake --build . --config Release
if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "Build completed successfully!"
echo "Executable: ./build/transpiler"
