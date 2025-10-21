cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(minipilot VERSION 1.0)

# Source files for minipilot
add_library(minipilot STATIC
    src/copter/copter_controller.cpp
    src/copter/copter_controller_pid.cpp
    src/copter/quadcopter.cpp
    src/link/handshake.cpp
    src/link/link.cpp
    src/link/rx.cpp
    src/link/tx.cpp
    src/link/handlers/handler_copter.cpp
    src/logging/logger.cpp
    src/logging/log_sink_link.cpp
    src/logging/log_sink_text.cpp
    src/parameters/parameter_manager.cpp
    src/sensors/accelerometer.cpp
    src/sensors/gyroscope.cpp
    src/state/ekf.cpp
    src/state/state_estimator.cpp
    src/telemetry/channels/channel_sensors.cpp
    src/telemetry/channels/channel_state.cpp
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
set(VENV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)
set(VENV_PIP ${VENV_DIR}/bin/pip)
set(VENV_PYTHON ${VENV_DIR}/bin/python3)
set(Python_EXECUTABLE "${VENV_PYTHON}")

if(NOT EXISTS ${VENV_DIR})
    # Create the virtual environment
    execute_process(
        COMMAND python3 -m venv ${VENV_DIR}
    )
    # Install the packages
    execute_process(
        COMMAND ${VENV_PIP} install -e ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Dependencies
add_subdirectory("lib/emblib")
add_subdirectory("lib/emblib/drivers/rtos/freertos")
add_subdirectory("lib/emblib/drivers/math/eigen")
add_subdirectory("lib/nanopb")
add_subdirectory("lib/minipilot-proto")

# Generate protobuf cpp files from sources
add_subdirectory("protobuf")

# Setup tests if working on this project
if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory("lib/pybind11")
    add_subdirectory("tests/binds")
endif()

# Link dependency libraries to minipilot
target_link_libraries(minipilot
PUBLIC
    emblib
    minipilot-proto-nanopb
)
