# Create a CMake dynamic library
pybind11_add_module(minipilot-binds ekf.cpp)

# Link minipilot to allow using mp files
target_link_libraries(minipilot-binds
PRIVATE
    minipilot
)
