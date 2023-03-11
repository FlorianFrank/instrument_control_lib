cmake_minimum_required(VERSION 3.4)
project(examples)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(SMUBasicUsage SMUBasicUsage.cpp)
target_include_directories(SMUBasicUsage PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include")
target_link_directories(SMUBasicUsage PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")
target_link_libraries(SMUBasicUsage PUBLIC instrument_control_lib)
install(TARGETS SMUBasicUsage
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )

add_executable(SMUBuffering SmuBuffering.cpp)
target_include_directories(SMUBuffering PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include")
target_link_directories(SMUBuffering PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")
target_link_libraries(SMUBuffering PUBLIC instrument_control_lib)
install(TARGETS SMUBuffering
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
