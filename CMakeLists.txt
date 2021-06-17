cmake_minimum_required(VERSION 3.4)
project(ce_device)

set(CMAKE_CXX_STANDARD 11)

add_library(ce_device SHARED
        src/device.cpp
        src/device.h
        src/spd1305.cpp
        src/spd1305.h
        src/kst33500.h
        src/kst33500.cpp
        src/kst3000.h
        src/kst3000.cpp
        )

install(TARGETS ce_device
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )

install(DIRECTORY src/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        )

# add_subdirectory(test)
