cmake_minimum_required(VERSION 3.13)

include("${CMAKE_CURRENT_SOURCE_DIR}/../build_config/python_config.cmake")

set(PROJECT_NAME "py_instrument_control_lib")
set(LIBRARY_NAME "lib${PROJECT_NAME}")


if(DEFINED Python_VERSION_MAJOR)
	find_package(Python3 ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR} COMPONENTS Interpreter Development )
	find_package(PythonLibs  ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR} REQUIRED )
else()
	find_package(Python3  COMPONENTS Interpreter Development )
	find_package(PythonLibs REQUIRED )

	set(Python_VERSION_MAJOR ${Python3_VERSION_MAJOR})
	set(Python_VERSION_MINOR ${Python3_VERSION_MINOR})
	set(Python3_ROOT_DIR ${Python3_STDLIB})
	set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
	set(PYTHON_INCLUDE_DIR ${Python3_INCLUDE_DIRS})
	set(PYTHON_LIB_DIR ${Python3_STDLIB}/lib)
endif()



add_definitions(/DBOOST_PYTHON_STATIC_LIB)
set(PYTHON_VERSION ${Python_VERSION_MAJOR}.${Python_VERSION_MINOR})
if (${Python_VERSION_MAJOR})
	message(STATUS "Using python version: ${PYTHON_VERSION}")
else()
	message(FATAL_ERROR "ERROR. Python not found.")
endif()

set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../boost)
set(BOOST_BOOTSTRAP_DIR ${BOOST_ROOT})

if (NOT EXISTS ${BOOST_ROOT})
	message("Create Boost ROOT DIR (${BOOST_ROOT})")
	execute_process(
			COMMAND bash "-c"  " mkdir -p ${BOOST_ROOT}"
			WORKING_DIRECTORY "."
			RESULT_VARIABLE ret)

	if(NOT ret EQUAL 0)
		message(FATAL_ERROR "Status ${ret}")
	endif()
endif()


#if (NOT IS_DIRECTORY ${BOOST_BOOTSTRAP_DIR})
	#message(FATAL_ERROR "Boost build directory not found. Run `git submodule update --init --recursive` First")
	if(WIN32)
	set(BOOST_CHECKOUT powershell 'git submodule update --init --recursive')
	else()
		set(BOOST_CHECKOUT bash "-c" " git submodule update --init --recursive")
	endif()
	execute_process(
			COMMAND ${BOOST_CHECKOUT}
			WORKING_DIRECTORY ${BOOST_ROOT}/..
			RESULT_VARIABLE ret
	)
	if(NOT ret EQUAL 0)
		message(WARNING "Status ${ret}")
	endif()
#endif()

# TODO: relocate
set(BOOST_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../boost/stage/lib)

if(WIN32)
	set(BOOST_BUILD_BINARY "${BOOST_BOOTSTRAP_DIR}/b2.exe")
	set(BOOST_BOOTSTRAP_COMMAND cmd /C ".\\bootstrap.bat gcc --with-python-version=${PYTHON_VERSION}")
	set(BOOST_BUILD_COMMAND cmd /C "${BOOST_BOOTSTRAP_DIR}/b2.exe toolset=gcc --with-python --prefix=build variant=release")
else()
	# TODO: BOOST_BUILD_BINARY
	set(BOOST_BUILD_COMMAND bash "-c" " export CPLUS_INCLUDE_PATH=\"$CPLUS_INCLUDE_PATH:${PYTHON_INCLUDE_DIR}\" && ${BOOST_BOOTSTRAP_DIR}/b2 toolset=gcc --with-python --prefix=build variant=release")
	set(BOOST_BOOTSTRAP_COMMAND bash "-c" " ./bootstrap.sh --with-python=/${PYTHON_EXECUTABLE} --with-python-version=${PYTHON_VERSION}")
endif()

if (NOT EXISTS ${BOOST_BUILD_BINARY})
	message(STATUS "Start to build boost build binary(b2) in: ${BOOST_BOOTSTRAP_DIR}")
	message(STATUS ${BOOST_BOOTSTRAP_COMMAND})
	execute_process(
			COMMAND ${BOOST_BOOTSTRAP_COMMAND}
			WORKING_DIRECTORY ${BOOST_BOOTSTRAP_DIR}
			RESULT_VARIABLE ret
	)
	if(NOT ret EQUAL 0)
		message(FATAL_ERROR "Status ${ret}")
	endif()
endif()
if (NOT IS_DIRECTORY ${BOOST_LIB_DIR} )
	message(STATUS "Start to build BOOST.PYTHON")
	message(STATUS ${BOOST_BUILD_COMMAND})
	execute_process(
			COMMAND ${BOOST_BUILD_COMMAND}
			WORKING_DIRECTORY ${BOOST_ROOT}
			RESULT_VARIABLE ret
	)
	if(NOT ret EQUAL 0)
		message(FATAL_ERROR "Status ${ret}")
	endif()
endif()




link_directories(${BOOST_LIB_DIR}/lib "${BOOST_ROOT}/stage/lib" ${PYTHON_LIB_DIR})
add_library(${PROJECT_NAME} MODULE boost_python_wrapper.cpp)

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

if(WIN32)
	set(BOOST_PYTHON_LIB "boost_python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}-mgw11-mt-x64-1_80")
else()
	set(BOOST_PYTHON_LIB "boost_python${Python_VERSION_MAJOR}${Python_VERSION_MINOR}")
endif()
target_link_directories(${PROJECT_NAME} PUBLIC "${BOOST_ROOT}/stage/lib/"
										       "${BOOST_ROOT}/libs/python/include")

target_link_libraries(${PROJECT_NAME} PUBLIC instrument_control_lib ${BOOST_PYTHON_LIB} python${Python_VERSION_MAJOR}.${Python_VERSION_MINOR})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
if(WIN32)
	set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".pyd")
	target_link_options(${PROJECT_NAME} PUBLIC -static -static-libgcc -static-libstdc++)
endif()

configure_file(instrument_control_lib.py instrument_control_lib.py COPYONLY)
configure_file(__init__.py __init__.py COPYONLY)