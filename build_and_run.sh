#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Run CMake to generate build files
cmake ..

# Build the project
cmake --build .

# Check if build was successful
if [ $? -eq 0 ]; then
    # Run the program
    ./backtest
else
    echo "Build failed"
fi

# Return to the original directory
cd ..