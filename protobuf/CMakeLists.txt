# Used to compile protobuf files on every build

# Path to nanopb generator
set(NANOPB_GEN "${PROJECT_SOURCE_DIR}/lib/nanopb/generator/nanopb_generator.py")

# Generated files are placed in the pb directory so that their
# includes would have a prefix to differentiate from source files
set(PROTO_SRC_DIR "${PROJECT_SOURCE_DIR}/lib/minipilot-proto/src")
set(PROTO_GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/gen/pb")

# Find all proto files recursively
# Look only in the src/mp folder
file(GLOB_RECURSE PROTO_FILES "${PROTO_SRC_DIR}/mp/*.proto")

# List of generated files
set(GENERATED_FILES "")

# Process each proto file
foreach(PROTO_FILE ${PROTO_FILES})
    # Get relative path to maintain directory structure
    file(RELATIVE_PATH REL_PATH ${PROTO_SRC_DIR} ${PROTO_FILE})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    get_filename_component(FILE_NAME ${PROTO_FILE} NAME_WE)
    
    # Define the output file and directory
    set(GEN_DIR ${PROTO_GEN_DIR}/${REL_DIR})
    set(GEN_FILE "${GEN_DIR}/${FILE_NAME}.pb.c")
    file(MAKE_DIRECTORY ${GEN_DIR})
    
    # Generate output files
    add_custom_command(
        OUTPUT ${GEN_FILE}
        DEPENDS ${PROTO_FILE}
        # nanopb generation
        COMMAND ${CMAKE_COMMAND} -E env 
            "PATH=${PROTOC_DIR}:$PATH"
            ${VENV_PYTHON} ${NANOPB_GEN}
            -I ${PROTO_SRC_DIR}
            -D ${PROTO_GEN_DIR}
            -q
            ${PROTO_FILE}
        # COMMENT "Generating nanopb files for ${REL_PATH}"
        VERBATIM
    )
    
    # Add to list of generated files
    list(APPEND GENERATED_FILES ${GEN_FILE})
endforeach()

# Create a library with all generated files
add_library(minipilot-proto-nanopb STATIC ${GENERATED_FILES})
target_link_libraries(minipilot-proto-nanopb PUBLIC protobuf-nanopb-static)
target_include_directories(minipilot-proto-nanopb PUBLIC ${PROTO_GEN_DIR})
target_include_directories(minipilot-proto-nanopb INTERFACE ${PROTO_GEN_DIR}/..)
