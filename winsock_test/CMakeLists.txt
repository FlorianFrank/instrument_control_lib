cmake_minimum_required(VERSION 3.22)
project(smu_socket)

set(CMAKE_CXX_STANDARD 14)

add_executable(smu_socket main.cpp)

if(MINGW)
    target_link_libraries(smu_socket ws2_32)
endif()
