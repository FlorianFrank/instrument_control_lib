cmake_minimum_required(VERSION 3.4)
project(test)

set(CMAKE_CXX_STANDARD 14)
message(STATUS "start building test...")

include_directories(../src /usr/local/Cellar/pugixml/1.11.4_2/include/)
link_directories(/usr/local/Cellar/pugixml/1.11.4_2/lib/)

add_executable(test main.cpp)

target_link_libraries(test ce_device pugixml)

