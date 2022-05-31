cmake_minimum_required(VERSION 3.4)
project(instrument_control_lib)

include("${CMAKE_CURRENT_SOURCE_DIR}/config/general_config.cmake")


add_definitions(-DDEVICE_CONFIG_FILE="${CMAKE_CURRENT_SOURCE_DIR}/config_files/SupportedDevices.xml")

set(CMAKE_CXX_STANDARD 11)

add_library(instrument_control_lib STATIC
        src/Device.cpp
        src/SPD1305.cpp
        src/KST33500.cpp
        src/KST3000.cpp
        src/ErrorHandler.cpp
        src/CommandLineInterface.cpp
        src/ConfigFileParser.cpp

        include/Device.h
        include/SPD1305.h
        include/KST33500.h
        include/KST3000.h
        include/CommandLineInterface.h
        include/ConfigFileParser.h
        )

target_compile_features(instrument_control_lib PRIVATE cxx_std_17)

target_include_directories(instrument_control_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include pugixml-1.12/src)

install(TARGETS instrument_control_lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        )

set(BUILD_SHARED_LIBS 1)
set(PUGI_XML_VERSION 1.12)
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pugixml-${PUGI_XML_VERSION}")

add_subdirectory(instrument_cmd_tool)
add_subdirectory(memristor_tester)
add_subdirectory(common_tools_lib)

target_link_libraries(instrument_control_lib PUBLIC pugixml-shared)