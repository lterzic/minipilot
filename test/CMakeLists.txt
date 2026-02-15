add_subdirectory(fw)

include(add-test.cmake)
add_test(logging)
add_test(telemetry)
add_test(sysmon)