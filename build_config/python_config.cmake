if(DEFINED LOCAL_BUILD)
    set(Python3_ROOT_DIR "/home/florianfrank/anaconda3/envs/Python38")

    set(PYTHON_EXECUTABLE "${Python3_ROOT_DIR}/bin/python3")
    set(PYTHON_INCLUDE_DIR "${Python3_ROOT_DIR}/include/python3.8/")
    set(PYTHON_LIB_DIR "${Python3_ROOT_DIR}/include/python3.8/lib")


    set(Python_VERSION_MAJOR 3)
    set(Python_VERSION_MINOR 8)
endif()