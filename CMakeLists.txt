cmake_minimum_required(VERSION 3.19)
project(ce_device)

set(CMAKE_CXX_STANDARD 14)

add_library(ce_device device.cpp device.h spd1305.cpp spd1305.h)

add_subdirectory(test)