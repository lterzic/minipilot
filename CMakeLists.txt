cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(minipilot VERSION 1.0)

# Source files for minipilot
add_library(minipilot STATIC
    src/config/configurator.cpp
    src/common/pb.cpp
    src/copter/copter_controller_task.cpp
    src/copter/copter_controller_pid.cpp
    src/copter/quadcopter.cpp
    src/link/handshake.cpp
    src/link/link.cpp
    src/link/rx.cpp
    src/link/tx.cpp
    src/link/modules/link_logging.cpp
    src/link/modules/link_telemetry.cpp
    src/logging/log_entry.cpp
    src/logging/logger.cpp
    src/sensors/accelerometer.cpp
    src/sensors/gyroscope.cpp
    src/sensors/sensor_manager.cpp
    src/state/estimator_runner.cpp
    src/telemetry/telemetry.cpp
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
PRIVATE
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
PRIVATE
    -flto
    -Wl,-Map=${CMAKE_PROJECT_NAME}.map
    -Wl,--gc-sections
    -Wl,--print-memory-usage
)

# FreeRTOS configuration
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

# Set the protoc path for nanopb
set(PROTOC_DIR ${PROJECT_SOURCE_DIR}/external/minipilot-proto/lib)
set(nanopb_PROTOC_PATH ${PROTOC_DIR}/protoc)

# Python environment setup
add_subdirectory("python")

# Dependencies
add_subdirectory("external/emblib")
add_subdirectory("external/emblib/drivers/rtos/freertos")
add_subdirectory("external/emblib/drivers/math/eigen")
add_subdirectory("external/nanopb-cpp")
add_subdirectory("external/minipilot-proto")

# Generate protobuf cpp files from sources
add_subdirectory("protobuf")

# Link dependency libraries to minipilot
target_link_libraries(minipilot
PUBLIC
    emblib
    emblib_math_eigen
    emblib_rtos_freertos
    minipilot-proto-nanopb
)
