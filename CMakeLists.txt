cmake_minimum_required(VERSION 3.4)

include("${CMAKE_CURRENT_SOURCE_DIR}/build_config/general_config.cmake")

project(instrument_control_lib)

file(GLOB source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB device_files ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/*.cpp)
file(GLOB device_type_files ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/types/*.cpp)



# BUILD instrument lib as
if(INSTRUMENT_LIB_BUILD_STATIC)
    add_library(instrument_control_lib STATIC ${source_files} ${device_files} ${device_type_files})
else() # build shared lib
    add_library(instrument_control_lib SHARED ${source_files} ${device_files} ${device_type_files})
endif()

# Build third party libraries and executables
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pugixml")
add_subdirectory(instrument_cmd_tool)
set(BoostRoot "${CMAKE_CURRENT_SOURCE_DIR}/boost")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/boost/include")
#add_subdirectory(memristor_tester)
add_subdirectory(common_tools_lib)
add_subdirectory(playground)

# TODO temporary disable python interface to satisfy CI
set(ENABLE_PYTHON 1)
if(ENABLE_PYTHON)
    add_subdirectory(py_icl)
endif() # ENABLE_PYTHON

if(BUILD_SHARED_LIBS)
    set(PUGIXML pugixml-shared)
else()
    set(PUGIXML pugixml-static)
endif()

if(PIL_SHARED)
    set(COMMON_TOOLS_LIB common_tools_lib)
else()
    set(COMMON_TOOLS_LIB common_tools_lib_static)
endif()

target_link_libraries(instrument_control_lib PUBLIC ${PUGIXML} ${COMMON_TOOLS_LIB})


target_include_directories(instrument_control_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                                          "${CMAKE_CURRENT_SOURCE_DIR}/pugixml-1.12/src"
                                                          "${CMAKE_CURRENT_SOURCE_DIR}/common_tools_lib/Communication/include"
                                                          "${CMAKE_CURRENT_SOURCE_DIR}/common_tools_lib/Additional"
                                                          "${CMAKE_CURRENT_SOURCE_DIR}/common_tools_lib/ErrorHandling/include"
                                                          "${CMAKE_CURRENT_SOURCE_DIR}/common_tools_lib/Logging/include")

install(TARGETS instrument_control_lib
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h"
        )
