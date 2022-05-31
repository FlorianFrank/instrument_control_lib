cmake_minimum_required(VERSION 3.4)
project(instrument_cmd_tool)

set(CMAKE_CXX_STANDARD 14)

add_executable(instrument_cmd_tool InstrumentCMDLineTool.cpp)

target_include_directories(instrument_cmd_tool PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_directories(instrument_cmd_tool PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")


target_link_libraries(instrument_cmd_tool pugixml instrument_control_lib)

install(TARGETS instrument_cmd_tool
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
