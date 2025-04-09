# Used to compile protobuf files on every build

# Path to protoc and nanopb generator
set(PROTOC_DIR "${PROJECT_SOURCE_DIR}/lib")
set(NANOPB_GEN "${PROJECT_SOURCE_DIR}/lib/nanopb/generator/nanopb_generator.py")

# Set paths
set(PROTO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(PROTO_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out")

# Find all proto files recursively
file(GLOB_RECURSE PROTO_FILES "${PROTO_SRC_DIR}/*.proto")

# Process each proto file
foreach(PROTO_FILE ${PROTO_FILES})
    # Get relative path to maintain directory structure
    file(RELATIVE_PATH REL_PATH ${PROTO_SRC_DIR} ${PROTO_FILE})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    get_filename_component(FILE_NAME ${PROTO_FILE} NAME_WE)
    
    # Create output directory
    set(OUT_C_DIR ${PROTO_OUT_DIR}/c/pb/${REL_DIR})
    set(OUT_PY_DIR ${PROTO_OUT_DIR}/py/${REL_DIR})
    file(MAKE_DIRECTORY ${OUT_C_DIR})
    file(MAKE_DIRECTORY ${OUT_PY_DIR})
    
    # Output files
    set(C_OUTPUT "${OUT_C_DIR}/${FILE_NAME}.pb.c")
    
    # Generate output files
    add_custom_command(
        OUTPUT ${C_OUTPUT}
        # nanopb generation
        COMMAND ${CMAKE_COMMAND} -E env 
            "PATH=${PROTOC_DIR}:$PATH"
            ${VENV_PYTHON} ${NANOPB_GEN}
            -I ${PROTO_SRC_DIR}
            -D ${PROTO_OUT_DIR}/c/pb
            -q
            ${PROTO_FILE}
        # py generation
        COMMAND ${PROTOC_DIR}/protoc
            -I=${PROTO_SRC_DIR}
            --python_out=${PROTO_OUT_DIR}/py
            ${PROTO_FILE}
        DEPENDS ${PROTO_FILE}
        # COMMENT "Generating nanopb files for ${REL_PATH}"
        VERBATIM
    )
    
    # Add to list of generated files (make accessible to parent scope)
    set_property(GLOBAL APPEND PROPERTY GENERATED_FILES ${C_OUTPUT})
endforeach()

# Get all generated files
get_property(ALL_GENERATED_FILES GLOBAL PROPERTY GENERATED_FILES)

# Create a library with all generated files
add_library(minipilot-proto STATIC ${ALL_GENERATED_FILES})
target_link_libraries(minipilot-proto PUBLIC protobuf-nanopb-static)
target_include_directories(minipilot-proto PUBLIC ${PROTO_OUT_DIR}/c/)
target_include_directories(minipilot-proto PUBLIC ${PROTO_OUT_DIR}/c/pb)
