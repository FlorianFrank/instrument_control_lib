function(build_documentation)
    # check if Doxygen is installed
    find_package(Doxygen)
    if (DOXYGEN_FOUND)

        # set input and output files
        set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/config_files/doc/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR})

        message("BINARY DIR " ${CMAKE_CURRENT_BINARY_DIR})

        # request to configure the file
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        message("Doxygen build started")

        add_custom_target(generate_documentation ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM)

    else (DOXYGEN_FOUND)
        message("Doxygen need to be installed to generate the doxygen documentation")
    endif (DOXYGEN_FOUND)
endfunction()
