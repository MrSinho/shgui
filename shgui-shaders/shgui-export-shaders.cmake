cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



function(build_shgui_export_shaders)



if (NOT DEFINED SH_GUI_BINARIES_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_BINARIES_DIR")
endif()

if (NOT DEFINED SH_GUI_ROOT_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_ROOT_DIR")
endif()



if (NOT TARGET native-export)
add_subdirectory(../externals/native-export/c EXCLUDE_FROM_ALL)
endif()



add_executable(shgui-export-shaders
    ${SH_GUI_ROOT_DIR}/shgui-shaders/src/export-shaders.c
)

target_link_libraries(
    shgui-export-shaders 
    PUBLIC 
    native-export
)

if (WIN32)
set_target_properties(shgui-export-shaders native-export PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/windows
    RUNTIME_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
)
else()
set_target_properties(shgui-export-shaders native-export PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
    RUNTIME_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
)
endif(WIN32)



endfunction()