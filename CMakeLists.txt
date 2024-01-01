cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



project(shgui C)



if (NOT TARGET native-export)
add_subdirectory(externals/native-export/c)
endif()

if (NOT TARGET shvulkan)
add_subdirectory(externals/shvulkan)
endif()


set(SH_GUI_VERSION      "pre-release")
set(SH_GUI_ROOT_DIR     ${CMAKE_CURRENT_SOURCE_DIR})



option(SH_GUI_BINARIES_DIR  CACHE OFF)
option(SH_GUI_BUILD_EXAMPLE CACHE OFF)



if (NOT SH_GUI_BINARIES_DIR)
set(SH_GUI_BINARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()



message(STATUS "SH_GUI_VERSION       : ${SH_GUI_VERSION}")
message(STATUS "SH_GUI_ROOT_DIR      : ${SH_GUI_ROOT_DIR}")
message(STATUS "SH_GUI_BINARIES_DIR  : ${SH_GUI_BINARIES_DIR}")
message(STATUS "SH_GUI_BUILD_EXAMPLE : ${SH_GUI_BUILD_EXAMPLE}")



include(shgui-shaders/shgui-shaders.cmake)
include(shgui-shaders/shgui-export-shaders.cmake)
include(shgui/shgui.cmake)
include(shgui-app/shgui-app.cmake)
include(shgui-example/shgui-example.cmake)
include(docs/shgui-docs.cmake)



function(main)

build_shgui()
build_shgui_shaders()
build_shgui_export_shaders()
build_shgui_app()
build_shgui_docs()

if(SH_GUI_BUILD_EXAMPLE)
build_shgui_example()
endif(SH_GUI_BUILD_EXAMPLE)

endfunction()



main()