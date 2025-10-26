# Create a CMake dynamic library
pybind11_add_module(state state.cpp)
pybind11_add_module(math math.cpp)

# Link minipilot to allow using mp files
target_link_libraries(state PRIVATE minipilot)
target_link_libraries(math PRIVATE minipilot)

# Override minipilot flags to include runtime type information
target_compile_options(minipilot
PUBLIC
    -frtti
)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-g -O0)

# Find the path to the venv site-packages folder
execute_process(
    COMMAND "${Python_EXECUTABLE}" -c
            "import site; print(site.getsitepackages()[0])"
    OUTPUT_VARIABLE VENV_SITE_PACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Create a .pth file to allow importing the bind python module
file(WRITE ${VENV_SITE_PACKAGES}/binds.pth ${CMAKE_CURRENT_BINARY_DIR}/../..)