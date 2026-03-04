# Find all cpp files and separate bindings
file(GLOB_RECURSE ALL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(SRC_FILES ${ALL_SRC_FILES})
set(BIND_FILES ${ALL_SRC_FILES})
list(FILTER SRC_FILES EXCLUDE REGEX ".*bindings\\.cpp$")
list(FILTER BIND_FILES INCLUDE REGEX ".*bindings\\.cpp$")

# Create a library with an include prefix
add_library(mp_algo ${SRC_FILES})
target_include_prefix(mp_algo PUBLIC mp/algo ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(mp_algo
PUBLIC
    emblib
    emblib_math_eigen
PRIVATE
    pybind11::pybind11
)

# Create a pybind11 module for each bindings.cpp, named after its parent folder
set(PYTHON_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/python/algo")
foreach(BIND_FILE ${BIND_FILES})
    get_filename_component(BIND_DIR "${BIND_FILE}" DIRECTORY)
    get_filename_component(MODULE_NAME "${BIND_DIR}" NAME)
    
    pybind11_add_module(${MODULE_NAME} ${BIND_FILE})
    target_link_libraries(${MODULE_NAME} PRIVATE mp_algo)

    file(RELATIVE_PATH REL_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${BIND_DIR}")
    set_target_properties(${MODULE_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PYTHON_PACKAGE_DIR}/${REL_DIR}/.."
    )
endforeach()
add_python_package(algo ${PYTHON_PACKAGE_DIR}/..)