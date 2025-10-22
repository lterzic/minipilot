# Create a CMake dynamic library
pybind11_add_module(mpbind binds.cpp)

# Override minipilot flags to include runtime type information
target_compile_options(minipilot
PUBLIC
    -frtti
)

# Link minipilot to allow using mp files
target_link_libraries(mpbind
PRIVATE
    minipilot
)

# Find the path to the venv site-packages folder
execute_process(
    COMMAND "${Python_EXECUTABLE}" -c
            "import site; print(site.getsitepackages()[0])"
    OUTPUT_VARIABLE VENV_SITE_PACKAGES
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Create a .pth file to allow importing the bind python module
file(WRITE ${VENV_SITE_PACKAGES}/binds.pth ${CMAKE_CURRENT_BINARY_DIR})