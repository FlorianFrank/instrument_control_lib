cmake_minimum_required(VERSION 3.4)
project(ce_device)

set(CMAKE_CXX_STANDARD 14)

add_library(ce_device SHARED
        device.cpp
        device.h
        spd1305.cpp
        spd1305.h
        kst33500.h
        kst33500.cpp
)

#add_subdirectory(test)