cmake_minimum_required(VERSION 3.4)
project(test)

set(CMAKE_CXX_STANDARD 14)
message(STATUS "start building test...")

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/pugixml")

add_executable(test main.cpp)
add_executable(InstrumentCmdLineTool InstrumentCmdLineTool.cpp)

target_include_directories(test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
                           "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_include_directories(InstrumentCmdLineTool PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
                                                         "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_directories(test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")
target_link_directories(InstrumentCmdLineTool PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")


target_link_libraries(test ce_device pugixml)
target_link_libraries(InstrumentCmdLineTool ce_device pugixml)

install(TARGETS test pugixml InstrumentCmdLineTool
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
