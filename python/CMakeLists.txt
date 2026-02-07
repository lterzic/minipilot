# Setup the python environment
set(VENV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)
set(VENV_PIP ${VENV_DIR}/bin/pip)
set(VENV_PYTHON ${VENV_DIR}/bin/python3)
set(Python_EXECUTABLE ${VENV_PYTHON} PARENT_SCOPE)

if(NOT EXISTS ${VENV_DIR})
    # Create the virtual environment
    execute_process(
        COMMAND python3 -m venv ${VENV_DIR}
    )
    # Install the packages
    execute_process(
        COMMAND ${VENV_PIP} install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    )
endif()

function(add_python_package NAME PATH)
    # Find the path to the venv site-packages folder
    execute_process(
        COMMAND "${Python_EXECUTABLE}" -c
                "import site; print(site.getsitepackages()[0])"
        OUTPUT_VARIABLE VENV_SITE_PACKAGES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    file(WRITE ${VENV_SITE_PACKAGES}/mp_${NAME}.pth ${PATH})
endfunction()
