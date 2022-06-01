cmake_minimum_required(VERSION 3.4)
project(test)

set(CMAKE_CXX_STANDARD 14)
message(STATUS "start building test...")


add_executable(memristor_tester main.cpp)

target_include_directories(memristor_tester PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/pugixml/src"
                           "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_directories(memristor_tester PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../out/lib")


target_link_libraries(memristor_tester PUBLIC instrument_control_lib pugixml)

install(TARGETS memristor_tester
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
