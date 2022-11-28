cmake_minimum_required(VERSION 3.4...3.18)

project(py_instrument_control_lib)

# TODO IF LOCAL
#set(PYTHON_LIBRARY_DIR "/home/florianfrank/anaconda3/envs/Python38/lib/python3.6/site-packages")
#set(PYTHON_EXECUTABLE "/home/florianfrank/anaconda3/envs/Python38/bin/python3")
#set(Python_ROOT_DIR "/home/florianfrank/anaconda3/envs/Python38")

add_subdirectory(pybind11)
pybind11_add_module(py_instrument_control_lib pybind_wrapper.cpp)

target_compile_definitions(py_instrument_control_lib
		PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

#if (WIN32)
target_link_libraries(${PROJECT_NAME} PUBLIC instrument_control_lib pybind11::module)
#endif()

target_include_directories(${PROJECT_NAME} PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../include"
		"${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include"
		"${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/Logging/include"
)

configure_file(testSMU.py testSMU.py COPYONLY)
