cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(minipilot VERSION 1.0)

# Source files for minipilot
add_library(minipilot STATIC
    src/link/handshake.cpp
    src/link/link.cpp
    src/link/rx.cpp
    src/link/tx.cpp
    src/link/link_modules/command_handler.cpp
    src/link/link_modules/link_log_sink.cpp
    src/link/link_modules/telemetry.cpp
    src/state/ekf.cpp
    src/state/state_estimator_periodic.cpp
    src/logging/logger.cpp
    src/logging/log_sink_text.cpp
    src/sensors/accelerometer.cpp
    src/sensors/gyroscope.cpp
    src/vehicles/copter.cpp
    src/vehicles/quadcopter.cpp
    src/control/copter_control.cpp
    src/control/copter_control_pid.cpp
)

# Library users should only use header files
# accessible through mp/ prefix
target_include_directories(minipilot
PUBLIC
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/include/mp"
INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
)

# Minipilot compiler and linker options
target_compile_options(minipilot
PUBLIC
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
target_link_options(minipilot
PUBLIC
    -flto
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# Add path to emblib configuration
add_library(emblib_config INTERFACE)
target_include_directories(emblib_config INTERFACE "${PROJECT_SOURCE_DIR}/src/libconfig")

# FreeRTOS configuration
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/src/libconfig)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

# Set the protoc path for nanopb
set(PROTOC_DIR ${PROJECT_SOURCE_DIR}/lib/minipilot-proto/lib)
set(nanopb_PROTOC_PATH ${PROTOC_DIR}/protoc)

# Setup the python environment
add_subdirectory("python")

# Dependencies
add_subdirectory("lib/emblib")
add_subdirectory("lib/emblib/drivers/rtos/freertos")
add_subdirectory("lib/emblib/drivers/math/eigen")
add_subdirectory("lib/nanopb")
add_subdirectory("lib/minipilot-proto")

# Generate protobuf cpp files from sources
add_subdirectory("protobuf")

# Link dependency libraries to minipilot
target_link_libraries(minipilot
PUBLIC
    emblib
    minipilot-proto-nanopb
)
