cmake_minimum_required(VERSION 3.15)

project(backtest VERSION 1.0.0 LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compiler warnings
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Option for build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# # Find packages
# find_package(fmt REQUIRED)  # Example of finding a package

# Copy data file to build directory
# Get a list of all .csv files in the source data directory
file(GLOB CSV_FILES "${CMAKE_SOURCE_DIR}/data/*.csv")

# Loop over each file and configure it
foreach(CSV_FILE ${CSV_FILES})
    get_filename_component(FILENAME ${CSV_FILE} NAME)
    configure_file(${CSV_FILE} 
                   ${CMAKE_BINARY_DIR}/data/${FILENAME} 
                   COPYONLY)
endforeach()


# Add executable
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/financial_metrics.cpp
    src/price_reader.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define project source directory
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
# Link libraries
# target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)

# Enable testing
# enable_testing()
# add_subdirectory(tests)

# Clang-tidy
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
if(ENABLE_CLANG_TIDY)
  find_program(CLANG_TIDY clang-tidy)
  if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
  else()
    message(WARNING "clang-tidy requested but executable not found")
  endif()
endif()

# Install rules
install(TARGETS ${PROJECT_NAME} DESTINATION bin)