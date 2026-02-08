# Path to nanopb generator
set(NANOPB_GEN "${PROJECT_SOURCE_DIR}/external/nanopb-cpp/generator/nanopb_generator.py")

# Generated files are placed in the pb directory so that their
# includes would have a prefix to differentiate from source files
set(PROTO_SRC_DIR "${PROJECT_SOURCE_DIR}/external/pblink/src")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gen")
file(REMOVE_RECURSE ${PROTO_GEN_DIR})

# Find all proto files recursively
file(GLOB_RECURSE SRC_FILES "${PROTO_SRC_DIR}/*.proto")

# Generate nanopb
set(PROTO_GEN_NANOPB_DIR "${PROTO_GEN_DIR}/nanopb")
file(MAKE_DIRECTORY ${PROTO_GEN_NANOPB_DIR})
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env 
    "PATH=${PROTOC_DIR}:$PATH"
    ${Python_EXECUTABLE} ${NANOPB_GEN}
    -I ${PROTO_SRC_DIR}
    -D ${PROTO_GEN_NANOPB_DIR}
    -e ".nanopb"
    -q
    -C
    --cpp-structs
    --cpp-oneof-enums
    ${SRC_FILES}
)

# Create a library with all generated files
file(GLOB_RECURSE NANOPB_FILES "${PROTO_GEN_NANOPB_DIR}/*.nanopb.c")
add_library(pblink_nanopb ${NANOPB_FILES})
target_link_libraries(pblink_nanopb PUBLIC protobuf-nanopb-static)
target_include_directories(pblink_nanopb PUBLIC ${PROTO_GEN_NANOPB_DIR})

# Generate python
set(PROTO_GEN_PY_DIR "${PROTO_GEN_DIR}/py")
file(MAKE_DIRECTORY ${PROTO_GEN_PY_DIR})
execute_process(COMMAND
    ${PROTOC}
    -I ${PROTO_SRC_DIR}
    --python_out=${PROTO_GEN_PY_DIR}
    --pyi_out=${PROTO_GEN_PY_DIR}
    ${SRC_FILES}
)
add_python_package(pblink ${PROTO_GEN_PY_DIR})