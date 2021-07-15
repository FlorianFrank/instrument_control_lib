cmake_minimum_required(VERSION 3.4)
project(test)

set(CMAKE_CXX_STANDARD 14)
message(STATUS "start building test...")

include_directories(../)

add_executable(test main.cpp measure_chip.cpp)

target_link_libraries(test ce_device)

