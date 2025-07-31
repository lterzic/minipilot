cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(minipilot VERSION 1.0)

# Source files for minipilot
add_library(minipilot STATIC
    src/comm/pb/pb_rx.cpp
    src/comm/pb/pb_tx.cpp
    src/comm/pb/telemetry.cpp
    src/comm/pb/log_sink_pb.cpp
    src/state/ekf.cpp
    src/logging/logger.cpp
    src/logging/log_sink_text.cpp
    src/sensors/accelerometer.cpp
    src/sensors/gyroscope.cpp
)

# Header files to be used by minipilot
target_include_directories(minipilot PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

# Header files to be used by external porting projects
target_include_directories(minipilot INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
)

# Minipilot compiler and linker options
target_compile_options(minipilot PUBLIC
    -fno-rtti
    -fno-exceptions
    -fdata-sections
    -ffunction-sections
    -fstack-usage
    -flto
    # -Wall
    # -Wextra
    # -Wpedantic
)
target_link_options(minipilot PUBLIC
    -flto
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# Add path to emblib configuration
add_library(emblib_config INTERFACE)
target_include_directories(emblib_config INTERFACE
    "${PROJECT_SOURCE_DIR}/src"
)

# FreeRTOS configuration
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

# Set the protoc path for nanopb
set(PROTOC_DIR ${PROJECT_SOURCE_DIR}/lib/minipilot-proto/lib)
set(nanopb_PROTOC_PATH ${PROTOC_DIR}/protoc)

# Setup the python environment
add_subdirectory("python")

# Dependencies
add_subdirectory("lib/emblib")
add_subdirectory("lib/emblib/impl/rtos/freertos")
add_subdirectory("lib/emblib/impl/math/eigen")
add_subdirectory("lib/nanopb")
add_subdirectory("lib/minipilot-proto")

# Generate protobuf cpp files from sources
add_subdirectory("protobuf")

# Link dependency libraries to minipilot
target_link_libraries(minipilot PUBLIC
    emblib
    minipilot-proto-nanopb
)