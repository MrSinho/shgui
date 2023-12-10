cmake_minimum_required(VERSION 3.13)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)


function(build_shgui)




if (NOT DEFINED SH_GUI_BINARIES_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_BINARIES_DIR")
endif()

if (NOT DEFINED SH_GUI_ROOT_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_ROOT_DIR")
endif()



if (NOT TARGET shvulkan)
add_subdirectory(../externals/shvulkan EXCLUDE_FROM_ALL)
endif()

add_library(shgui 
    ${SH_GUI_ROOT_DIR}/shgui/src/shgui.c
)

target_include_directories(shgui PUBLIC 
    ${SH_GUI_ROOT_DIR}/shgui/include
    ${SH_GUI_ROOT_DIR}/shaders/include
    ${SH_GUI_ROOT_DIR}/shgui-shaders/include
)

target_link_libraries(shgui PUBLIC shvulkan)

if (WIN32)
set_target_properties(shgui shvulkan PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/windows
)
else()
set_target_properties(shgui shvulkan PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
)
endif(WIN32)

endfunction()