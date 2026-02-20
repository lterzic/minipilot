add_subdirectory(fw)

function (add_test TEST_NAME)
    set(TARGET_NAME "mp_test_${TEST_NAME}")
    set(TEST_PATH "tests/${TEST_NAME}")
    file(GLOB FW_SRC_FILES "${TEST_PATH}/*.cpp")
    
    add_executable(${TARGET_NAME} ${FW_SRC_FILES})

    target_link_libraries(${TARGET_NAME}
    PRIVATE
        -Wl,--whole-archive mp_test_fw -Wl,--no-whole-archive
    )
endfunction()

add_test(logging)
add_test(telemetry)
add_test(sysmon)
add_test(config)