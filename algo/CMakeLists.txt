# Find all cpp files and separate bindings
file(GLOB_RECURSE ALL_SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
set(SRC_FILES ${ALL_SRC_FILES})
set(BIND_FILES ${ALL_SRC_FILES})
list(FILTER SRC_FILES EXCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/*bindings.cpp")
list(FILTER BIND_FILES INCLUDE REGEX "${CMAKE_CURRENT_SOURCE_DIR}/*bindings.cpp")

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

# message(STATUS "SRC: ${SRC_FILES} \n\n BIND: ${BIND_FILES}")
