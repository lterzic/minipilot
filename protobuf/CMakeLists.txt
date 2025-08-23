# Used to compile protobuf files on every build

# Path to nanopb generator
set(NANOPB_GEN "${PROJECT_SOURCE_DIR}/lib/nanopb/generator/nanopb_generator.py")

# Generated files are placed in the pb directory so that their
# includes would have a prefix to differentiate from source files
set(PROTO_SRC_DIR "${PROJECT_SOURCE_DIR}/lib/minipilot-proto/src")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gen/pb")

# Find all proto files recursively
file(GLOB_RECURSE SRC_FILES "${PROTO_SRC_DIR}/*.proto")

# Generate files
file(REMOVE_RECURSE ${PROTO_GEN_DIR})
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env 
    "PATH=${PROTOC_DIR}:$PATH"
    ${VENV_PYTHON} ${NANOPB_GEN}
    -I ${PROTO_SRC_DIR}
    -D ${PROTO_GEN_DIR}
    -e ".nanopb"
    -q
    ${SRC_FILES}
)

# Create a library with all generated files
file(GLOB_RECURSE GEN_FILES "${PROTO_GEN_DIR}/*.nanopb.c")
add_library(minipilot-proto-nanopb STATIC ${GEN_FILES})
target_link_libraries(minipilot-proto-nanopb PUBLIC protobuf-nanopb-static)
target_include_directories(minipilot-proto-nanopb PUBLIC ${PROTO_GEN_DIR})
target_include_directories(minipilot-proto-nanopb INTERFACE ${PROTO_GEN_DIR}/..)
