cmake_minimum_required(VERSION 3.4)
project(playground)

set(CMAKE_CXX_STANDARD 14)
message(STATUS "start building test...")

add_executable(playground main.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

target_include_directories(playground PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_directories(playground PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")

target_link_libraries(playground PUBLIC instrument_control_lib pugixml)

install(TARGETS playground
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )