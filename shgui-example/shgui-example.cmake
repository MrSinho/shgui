cmake_minimum_required(VERSION 3.13)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



function(build_shgui_example)



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
add_subdirectory(../externals/glfw)
endif()

add_executable(shgui-example 
    ${SH_GUI_ROOT_DIR}/shgui-example/src/shgui-example.c
)

target_link_libraries(shgui-example PUBLIC 
    shgui-app
)

if (WIN32)
set_target_properties(shgui-example shgui-app shgui shvulkan glfw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/windows
    RUNTIME_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/windows
)
else()
set_target_properties(shgui-example shgui-app shgui shvulkan glfw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
    RUNTIME_OUTPUT_DIRECTORY ${SH_GUI_BINARIES_DIR}/linux
)
endif(WIN32)

endfunction()