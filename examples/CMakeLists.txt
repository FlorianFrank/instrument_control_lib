cmake_minimum_required(VERSION 3.4)
project(examples)

set(CMAKE_CXX_STANDARD 14)

add_executable(examples MeasurementBuffering.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

target_include_directories(examples PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include")

target_link_directories(examples PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")

target_link_libraries(examples PUBLIC instrument_control_lib)

install(TARGETS examples
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
