cmake_minimum_required(VERSION 3.4)
project(playground)

set(CMAKE_CXX_STANDARD 14)

add_executable(playground main.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

target_include_directories(playground PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include")

target_link_directories(playground PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")

target_link_libraries(playground PUBLIC instrument_control_lib)

install(TARGETS playground
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )


project(measure_chip)
add_executable(measure_chip measure_chip.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

target_include_directories(measure_chip PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include")

target_link_directories(measure_chip PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")

target_link_libraries(measure_chip PUBLIC instrument_control_lib)

install(TARGETS measure_chip
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )


project(memristor_tester)
add_executable(memristor_tester memristor_tester.cpp)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

target_include_directories(memristor_tester PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
        "${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include")

target_link_directories(memristor_tester PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")

target_link_libraries(memristor_tester PUBLIC instrument_control_lib)

install(TARGETS memristor_tester
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )