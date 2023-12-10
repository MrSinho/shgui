cmake_minimum_required(VERSION 3.13)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



function(build_shgui_app)



if (NOT DEFINED SH_GUI_BINARIES_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_BINARIES_DIR")
endif()

if (NOT DEFINED SH_GUI_ROOT_DIR)
message(FATAL_ERROR "shgui cmake error: missing SH_GUI_ROOT_DIR")
endif()



if (NOT TARGET shvulkan)
add_subdirectory(../externals/shvulkan EXCLUDE_FROM_ALL)
endif()

if (NOT TARGET shgui)
include(../shgui/shgui.cmake)
endif()

if (NOT TARGET glfw)
add_subdirectory(externals/glfw)
endif()



add_library(shgui-app
    ${SH_GUI_ROOT_DIR}/shgui-app/src/shgui-app.c
)

target_include_directories(
    shgui-app PUBLIC
    ${SH_GUI_ROOT_DIR}/shgui-app/include
)

target_link_libraries(shgui-app PUBLIC 
    shgui shvulkan glfw
)

if (WIN32)
set_target_properties(shgui-app shgui shvulkan glfw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/windows
    RUNTIME_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
)
else()
set_target_properties(shgui-app shgui shvulkan glfw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
    RUNTIME_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
)
endif(WIN32)

endfunction()