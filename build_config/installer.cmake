if(WIN32)
    set (CPACK_GENERATOR "NSIS")

    set(CPACK_PACKAGE_NAME "instrument_control_lib")
    set(CPACK_PACKAGE_VERSION "1.0")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
    set(INSTALL_DIR .)
    set(LIB_DIR .)
    set(EXT_DIR .)
endif()