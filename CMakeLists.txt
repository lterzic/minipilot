cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(minipilot VERSION 1.0)

# emblib configuration
add_library(emblib_config INTERFACE)
target_include_directories(emblib_config INTERFACE
    "${PROJECT_SOURCE_DIR}/src"
)

# Set the protoc path for nanopb
set(nanopb_PROTOC_PATH ${PROJECT_SOURCE_DIR}/lib/protoc)

# Setup the python environment
add_subdirectory("python")

# Dependencies
add_subdirectory("lib/emblib")
add_subdirectory("lib/nanopb")

# Generate protobuf cpp files from sources
add_subdirectory("protobuf")

# Source files for minipilot
add_library(minipilot STATIC
    src/vehicles/copter/copter.cpp
    src/vehicles/copter/quadcopter.cpp
    src/vehicles/copter/control/copter_controller_pid.cpp
    src/tasks/task_accelerometer.cpp
    src/tasks/task_gyroscope.cpp
    src/tasks/task_logger.cpp
    src/tasks/task_telemetry.cpp
    src/tasks/task_state_estimator.cpp
    src/tasks/task_receiver.cpp
    src/tasks/task_vehicle.cpp
    src/state/ekf_ahrs.cpp
    src/state/ekf_inertial.cpp
    src/util/logger.cpp
    src/main.cpp
)

# Header files to be used by minipilot
target_include_directories(minipilot PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
)

# Header files to be used by external porting projects
target_include_directories(minipilot INTERFACE
    "${PROJECT_SOURCE_DIR}/include"
)

# Link dependency libraries to minipilot
target_link_libraries(minipilot PUBLIC
    emblib
    minipilot-proto
)

# Minipilot compile options
target_compile_options(minipilot PUBLIC
    -fno-rtti
    -fno-exceptions
)