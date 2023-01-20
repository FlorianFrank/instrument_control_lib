cmake_minimum_required(VERSION 3.4)

project(instrument_control_lib_unit_tests)

set(device_unit_test_files "${CMAKE_CURRENT_SOURCE_DIR}/DeviceTest.cpp")
add_executable(device_unit_test ${device_unit_test_files})

enable_testing()
include(GoogleTest)
gtest_discover_tests("device_unit_test")


target_link_libraries(device_unit_test gtest_main gtest instrument_control_lib)
target_include_directories(device_unit_test PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../pugixml-1.12/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/FileHandling/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/Communication/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/Threading/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/Additional"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/Logging/include")
