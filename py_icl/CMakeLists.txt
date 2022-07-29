cmake_minimum_required(VERSION 3.4)

set(PROJECT_NAME "py_icl")
set(LIBRARY_NAME "lib${PROJECT_NAME}")

set(PYTHON_FOLDER "C:/Users/Administrator/AppData/Local/Programs/Python/Python310")

find_package (Python REQUIRED)
find_package(PythonLibs)

add_definitions(/DBOOST_PYTHON_STATIC_LIB)
set(PYTHON_VERSION ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR})
if (${Python_VERSION_MAJOR})
	message(STATUS "Using python version: ${PYTHON_VERSION}")
else()
	message(FATAL_ERROR "ERROR. Python not found.")
endif()


set(BOOST_BOOTSTRAP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../boost/tools/build)

message(${BOOST_BOOTSTRAP_DIR})
if (NOT IS_DIRECTORY ${BOOST_BOOTSTRAP_DIR})
	message(FATAL_ERROR "Boost build directory not found. Run `git submodule update --init --recursive` First")
	set(BOOST_CHECKOUT powershell 'git submodule update --init --recursive')
	execute_process(
			COMMAND ${BOOST_CHECKOUT}
			WORKING_DIRECTORY ${BOOST_ROOT}
	)
endif()

# TODO: relocate
set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../boost)
set(BOOST_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../boost/build)

if(WIN32)
	set(BOOST_BUILD_BINARY "${BOOST_BOOTSTRAP_DIR}/b2.exe")
	set(BOOST_BOOTSTRAP_COMMAND cmd /C ".\\bootstrap.bat gcc --with-python-version=${PYTHON_VERSION}")
	set(BOOST_BUILD_COMMAND cmd /C ".\\tools\\build\\b2.exe toolset=gcc --with-python --prefix=build variant=release")
else()
	# TODO: BOOST_BUILD_BINARY
	set(BOOST_BOOTSTRAP_COMMAND "./bootstrap.sh --with-python-version=${PYTHON_VERSION}")
endif()

if (NOT EXISTS ${BOOST_BUILD_BINARY})
	message(STATUS "Start to build boost build binary(b2) in: ${BOOST_BOOTSTRAP_DIR}")
	message(STATUS ${BOOST_BOOTSTRAP_COMMAND})
	execute_process(
			COMMAND ${BOOST_BOOTSTRAP_COMMAND}
			WORKING_DIRECTORY ${BOOST_BOOTSTRAP_DIR}
	)
endif()
if (NOT IS_DIRECTORY ${BOOST_LIB_DIR} )
	message(STATUS "Start to build BOOST.PYTHON")
	message(STATUS ${BOOST_BUILD_COMMAND})
	execute_process(
			COMMAND ${BOOST_BUILD_COMMAND}
			WORKING_DIRECTORY ${BOOST_ROOT}
	)
endif()


if (NOT IS_DIRECTORY ${BOOST_LIB_DIR})
	#	message("Start to build Boost library")
	#	message(${BOOST_BOOTSTRAP_COMMAND})
	#	execute_process(
	#		COMMAND ${BOOST_BOOTSTRAP_COMMAND}
	#		WORKING_DIRECTORY ${BOOST_ROOT}
	#		OUTPUT_FILE bootstrap_boost.txt
	#	)
	#
	#	message(${BOOST_BUILD_COMMAND})
	#	execute_process(
	#		COMMAND ${BOOST_BUILD_COMMAND}
	#		WORKING_DIRECTORY ${BOOST_ROOT}
	#		OUTPUT_FILE build_boost.txt
	#	)
else()
	message("Boost library built.")
endif()

link_directories(${BOOST_LIB_DIR}/lib "${PYTHON_FOLDER}"
		"${BOOST_ROOT}/stage/lib")
add_library(${PROJECT_NAME} SHARED main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE
		"${BOOST_ROOT}"
		"${BOOST_LIB_DIR}/include/boost-1_80"
		"${PYTHON_INCLUDE_PATH}"
		"/usr/include/python${PYTHON_VERSION}"
		"${CMAKE_CURRENT_SOURCE_DIR}/../include"
		"${CMAKE_CURRENT_SOURCE_DIR}/../boost"
		"${CMAKE_CURRENT_SOURCE_DIR}/../common_tools_lib/ErrorHandling/include"
		)


set(BOOST_PYTHON_LIB "boost_python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")
target_link_directories(${PROJECT_NAME} PUBLIC "${BOOST_ROOT}\\stage\\lib")

set(BOOST_PYTHON_LIB "boost_python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}-mgw11-mt-x64-1_80")
message("BOOST PYTHON LIB ${BOOST_PYTHON_LIB}")
target_link_directories(${PROJECT_NAME} PUBLIC "${BOOST_ROOT}/stage/lib/"
										       "${BOOST_ROOT}/libs/python/include")

target_link_libraries(${PROJECT_NAME} PUBLIC instrument_control_lib ${BOOST_PYTHON_LIB} ${PYTHON_LIBRARY})
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
target_link_options(${PROJECT_NAME} PUBLIC -static -static-libgcc -static-libstdc++)

configure_file(py_icl_test.py py_icl_test.py COPYONLY)