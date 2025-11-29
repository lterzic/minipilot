# Setup the python environment
set(VENV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)
set(VENV_PIP ${VENV_DIR}/bin/pip)
set(VENV_PYTHON ${VENV_DIR}/bin/python3)
set(Python_EXECUTABLE "${VENV_PYTHON}")
set(Python_EXECUTABLE "${VENV_PYTHON}" PARENT_SCOPE)

if(NOT EXISTS ${VENV_DIR})
    # Create the virtual environment
    execute_process(
        COMMAND python3 -m venv ${VENV_DIR}
    )
    # Install the packages
    execute_process(
        COMMAND ${VENV_PIP} install -e ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()

# Setup bindings if this is not a submodule
if (PROJECT_IS_TOP_LEVEL)
    add_subdirectory("${PROJECT_SOURCE_DIR}/external/pybind11" "${PROJECT_BINARY_DIR}/external/pybind11")
    add_subdirectory("mp/binds")
endif()