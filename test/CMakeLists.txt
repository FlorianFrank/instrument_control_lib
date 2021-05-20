cmake_minimum_required(VERSION 3.19)
project(test)

set(CMAKE_CXX_STANDARD 14)
message(STATUS "start building test...")

include_directories(../)
link_directories(../out/)

add_executable(test main.cpp)

target_link_libraries(test ce_device)

