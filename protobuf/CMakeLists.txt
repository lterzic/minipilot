# Used to compile protobuf files on every build

# Path to protoc and nanopb generator
set(PROTOC_DIR "${CMAKE_SOURCE_DIR}/lib")
set(NANOPB_GEN "${CMAKE_SOURCE_DIR}/lib/nanopb/generator")

# Set paths
set(PROTO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/c/pb")

# Find all proto files recursively
file(GLOB_RECURSE PROTO_FILES "${PROTO_SRC_DIR}/*.proto")

# Create output directories
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

# Process each proto file
foreach(PROTO_FILE ${PROTO_FILES})
    # Get relative path to maintain directory structure
    file(RELATIVE_PATH REL_PATH ${PROTO_SRC_DIR} ${PROTO_FILE})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    get_filename_component(FILE_NAME ${PROTO_FILE} NAME_WE)
    
    # Create output directory
    set(OUTPUT_DIR "${PROTO_GEN_DIR}/${REL_DIR}")
    file(MAKE_DIRECTORY ${OUTPUT_DIR})
    
    # Output files
    set(C_OUTPUT "${OUTPUT_DIR}/${FILE_NAME}.pb.c")
    set(H_OUTPUT "${OUTPUT_DIR}/${FILE_NAME}.pb.h")
    
    # Command to generate nanopb files
    add_custom_command(
        OUTPUT ${C_OUTPUT} ${H_OUTPUT}
        COMMAND ${CMAKE_COMMAND} -E env 
            "PATH=${PROTOC_DIR}:$PATH PYTHONPATH=${NANOPB_GEN}"
            ${VENV_PYTHON} ${NANOPB_GEN}/nanopb_generator.py
            -I ${PROTO_SRC_DIR}
            -D ${PROTO_GEN_DIR}
            -q
            ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        COMMENT "Generating nanopb files for ${REL_PATH}"
        VERBATIM
    )
    
    # Add to list of generated files (make accessible to parent scope)
    set_property(GLOBAL APPEND PROPERTY GENERATED_FILES ${C_OUTPUT} ${H_OUTPUT})
endforeach()

# Get all generated files
get_property(ALL_GENERATED_FILES GLOBAL PROPERTY GENERATED_FILES)

# Create a library with all generated files
add_library(minipilot-proto STATIC ${ALL_GENERATED_FILES})
target_link_libraries(minipilot-proto PUBLIC protobuf-nanopb-static)
target_include_directories(minipilot-proto PUBLIC ${PROTO_GEN_DIR})
target_include_directories(minipilot-proto PUBLIC ${PROTO_GEN_DIR}/..)
