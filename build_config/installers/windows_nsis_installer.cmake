function(build_nsis_installer)

    install(TARGETS instrument_control_lib
            LIBRARY DESTINATION       "lib"
            ARCHIVE DESTINATION       "lib"
            RESOURCE DESTINATION      "lib"
            RUNTIME DESTINATION        "bin"
            PUBLIC_HEADER DESTINATION "include")


    set(CPACK_PACKAGE_NAME "instrument_control_lib")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Library to control instruments like oscilloscopes and SMUs via TCP and SCPI commands")

    set(CPACK_PACKAGE_VERSION_MAJOR 1)
    set(CPACK_PACKAGE_VERSION_MINOR 0)
    set(CPACK_PACKAGE_VERSION_PATCH 0)

    set(CPACK_PACKAGE_INSTALL_DIRECTORY ${INSTALL_DIR})
    set(CPACK_PACKAGE_VENDOR "University of Passau - Faculty for computer science and mathematics")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_SOURCE_GENERATOR "TGZ")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "instrument_control_lib_1.0.0")

    set (CPACK_VERBATIM_VARIABLES ON)

    #set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_LIST_DIR}/cmake/installers/nsis_bundle/logo.ico")
    #set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_LIST_DIR}/cmake/installers/nsis_bundle/logo.ico")
    #set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP  "${CMAKE_CURRENT_LIST_DIR}/cmake/installers/nsis_bundle/sidebar.bmp")
    #set(CPACK_NSIS_MUI_HEADERIMAGE "${CMAKE_CURRENT_LIST_DIR}/cmake/installers/nsis_bundle/header.bmp")
    #string(REGEX REPLACE "/" "\\\\" CPACK_NSIS_MUI_ICON "${CPACK_NSIS_MUI_ICON}")
    #string(REGEX REPLACE "/" "\\\\" CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP}")
    #string(REGEX REPLACE "/" "\\\\" CPACK_NSIS_MUI_HEADERIMAGE "${CPACK_NSIS_MUI_HEADERIMAGE}")

    set(CPACK_NSIS_URL_INFO_ABOUT "https://florianfrank.github.io/instrument_control_lib/")
    set(CPACK_NSIS_CONTACT "Florian.Frank@uni-passau.de")

    include(CPack)
endfunction() # build_nsis_installer




