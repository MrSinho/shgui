cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



project(shgui C)



if (NOT TARGET native-export)
add_subdirectory(externals/native-export/c)
endif()

if (NOT TARGET shvulkan)
add_subdirectory(externals/shvulkan)
endif()


set(SH_GUI_VERSION  "v1.0.0")
set(SH_GUI_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})



option(SH_GUI_BUILD_EXAMPLE CACHE OFF)
set(SH_GUI_BINARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin CACHE STRING "Default SH_GUI_BINARIES_DIR")



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