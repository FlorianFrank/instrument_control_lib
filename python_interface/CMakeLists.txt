cmake_minimum_required(VERSION 3.4...3.18)
project(py_instrument_control_lib)

add_subdirectory(pybind11)
pybind11_add_module(py_instrument_control_lib pybind_wrapper.cpp)

target_compile_definitions(py_instrument_control_lib
		PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

target_include_directories(${PROJECT_NAME} PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../include"
		"${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/Logging/include"
)
