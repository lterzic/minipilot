function (add_test TEST_NAME)
    set(TEST_PATH "tests/${TEST_NAME}")
    set(TARGET_NAME "mp_test_${TEST_NAME}")

    set(FW_SRC_PATH "${TEST_PATH}")
    file(GLOB FW_SRC_FILES "${FW_SRC_PATH}/*.cpp")
    
    add_executable(${TARGET_NAME} ${FW_SRC_FILES})

    target_link_libraries(${TARGET_NAME}
    PRIVATE
        -Wl,--whole-archive mp_test_fw -Wl,--no-whole-archive
    )
endfunction()