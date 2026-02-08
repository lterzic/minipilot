add_library(mp_test_fw
    globals.cpp
    init.cpp
)

target_include_directories(mp_test_fw
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mp_test_fw
    minipilot
    emblib_io_net
    emblib_io_std
)
