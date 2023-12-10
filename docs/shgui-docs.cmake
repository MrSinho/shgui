cmake_minimum_required(VERSION 3.13)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



# 
# DOCS GENERATION
# 
function(build_shgui_docs)




if (NOT DEFINED SH_GUI_BINARIES_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_BINARIES_DIR")
endif()

if (NOT DEFINED SH_GUI_ROOT_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_ROOT_DIR")
endif()




find_package(Doxygen)


if (DOXYGEN_FOUND)

    set(SH_GUI_DOXYFILE_SRC ${SH_GUI_ROOT_DIR}/docs/Doxyfile.in)
    set(SH_GUI_DOXYFILE_DST ${SH_GUI_ROOT_DIR}/docs/Doxyfile)

    message(STATUS "Configuring " ${SH_GUI_DOXYFILE_DST} " from " ${SH_GUI_DOXYFILE_SRC})

    configure_file(
        ${SH_GUI_DOXYFILE_SRC} 
        ${SH_GUI_DOXYFILE_DST}
    )

    message(STATUS "Creating custom docs command: " ${DOXYGEN_EXECUTABLE} " " ${SH_GUI_DOXYFILE_DST})

    add_custom_target(docs-shgui
        COMMAND ${DOXYGEN_EXECUTABLE} ${SH_GUI_DOXYFILE_DST}
        WORKING_DIRECTORY ${SH_GUI_ROOT_DIR}
        COMMENT "Generating HTML documentation"
        VERBATIM
    )

endif(DOXYGEN_FOUND)

endfunction()