cmake_minimum_required(VERSION 3.4)
project(ce_device)

set(CMAKE_CXX_STANDARD 11)

add_library(ce_device SHARED
        device.cpp
        device.h
        spd1305.cpp
        spd1305.h
        kst33500.h
        kst33500.cpp
)

set(CMAKE_INSTALL_PREFIX, "./out")

install(TARGETS ce_device
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        PUBLIC_HEADER DESTINATION include
        )

#add_subdirectory(test)
