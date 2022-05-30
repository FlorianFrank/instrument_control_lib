cmake_minimum_required(VERSION 3.4)
project(ce_device)

set(CMAKE_CXX_STANDARD 11)

add_library(ce_device STATIC
        src/device.cpp
        src/spd1305.cpp
        src/kst33500.cpp
        src/kst3000.cpp
        src/error_handler.cpp
        src/command_line_interface.cpp

        include/device.h
        include/spd1305.h
        include/kst33500.h
        include/kst3000.h
        include/command_line_interface.h
        )

target_compile_features(ce_device PRIVATE cxx_std_17)



target_include_directories(ce_device PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

install(TARGETS ce_device
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        )

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(instrument_cmd_tool)
    add_subdirectory(memristor_tester)
endif ()
