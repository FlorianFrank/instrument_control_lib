cmake_minimum_required(VERSION 3.4)

set(PROJECT_NAME "py_icl")
set(LIBRARY_NAME "lib${PROJECT_NAME}")

find_package (Python)

set(PYTHON_VERSION ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR})
message("Using python version: ${PYTHON_VERSION}")


set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../boost)
set(BOOST_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../boost/stage/lib)
set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh --with-python-version=${PYTHON_VERSION})
set(BOOST_BUILD_COMMAND ./b2 --with-python)

if (NOT IS_DIRECTORY ${BOOST_LIB_DIR})
	message("Start to build Boost library")
	message(${BOOST_BOOTSTRAP_COMMAND})
	execute_process(
		COMMAND ${BOOST_BOOTSTRAP_COMMAND}
		WORKING_DIRECTORY ${BOOST_ROOT}
		OUTPUT_FILE bootstrap_boost.txt
	)

	message(${BOOST_BUILD_COMMAND})
	execute_process(
		COMMAND ${BOOST_BUILD_COMMAND}
		WORKING_DIRECTORY ${BOOST_ROOT}
		OUTPUT_FILE build_boost.txt
	)
else()
	message("Boost library built.")
endif()

link_directories(${BOOST_LIB_DIR})

add_library(${PROJECT_NAME} SHARED main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
	"/usr/include/python${PYTHON_VERSION}"
	"${CMAKE_CURRENT_SOURCE_DIR}/../include"
	"${CMAKE_CURRENT_SOURCE_DIR}/../boost"
	"${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include"
)

set(BOOST_PYTHON_LIB "boost_python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")

target_link_libraries(${PROJECT_NAME} PUBLIC instrument_control_lib ${BOOST_PYTHON_LIB})

configure_file(py_icl_test.py py_icl_test.py COPYONLY)