cmake_minimum_required(VERSION 3.4)

include("${CMAKE_CURRENT_SOURCE_DIR}/build_config/general_config.cmake")

project(instrument_control_lib)

file(GLOB source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

# BUILD instrument lib as
if(INSTRUMENT_LIB_BUILD_STATIC)
    add_library(instrument_control_lib STATIC ${source_files})
else() # build shared lib
    add_library(instrument_control_lib SHARED ${source_files})
endif()

# Build third party libraries and executables
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pugixml-${PUGI_XML_VERSION}")
add_subdirectory(instrument_cmd_tool)
add_subdirectory(memristor_tester)
add_subdirectory(common_tools_lib)

if(PIL_SHARED)
    target_link_libraries(instrument_control_lib PUBLIC pugixml-shared common_tools_lib)
else()
    target_link_libraries(instrument_control_lib PUBLIC pugixml-shared common_tools_lib_static)
endif()

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