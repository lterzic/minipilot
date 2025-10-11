# Used to setup the python environment used by the project

set(VENV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)
set(VENV_DIR ${VENV_DIR} PARENT_SCOPE)
set(VENV_PIP ${VENV_DIR}/bin/pip)
set(VENV_PYTHON ${VENV_DIR}/bin/python3 PARENT_SCOPE)

if(NOT EXISTS ${VENV_DIR})
    # Create the virtual environment
    execute_process(
        COMMAND python3 -m venv ${VENV_DIR}
    )
endif()

# Install the packages
execute_process(
    COMMAND ${VENV_PIP} install -e ${CMAKE_CURRENT_SOURCE_DIR}
)
