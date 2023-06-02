cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



project(shgui C)



if (NOT TARGET native-export)
add_subdirectory(externals/native-export/c)
endif()

if (NOT TARGET shvulkan)
add_subdirectory(externals/shvulkan)
endif()



function(build_shgui)
add_library(shgui 
    shgui/src/shgui.c
)
target_include_directories(shgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/shgui/include
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/include
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts/include
)
target_link_libraries(shgui PUBLIC shvulkan)
set_target_properties(shgui shvulkan PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
endfunction()



function(build_export_shaders)
add_executable(export-shaders
    shaders/src/export-shaders.c
)
target_link_libraries(export-shaders PUBLIC native-export)
endfunction()



function(build_shgui_app)
if (NOT TARGET glfw)
add_subdirectory(externals/glfw)
endif()
add_library(shgui-app
    shgui-app/src/shgui-app.c
)
target_include_directories(
    shgui-app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/shgui-app/include
)
target_link_libraries(shgui-app PUBLIC 
    shgui shvulkan glfw
)
set_target_properties(shgui-app shgui shvulkan glfw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
endfunction()



function(build_shgui_example)
if (NOT TARGET glfw)
add_subdirectory(externals/glfw)
endif()
add_executable(shgui-example 
    shgui-example/src/shgui-example.c
)
target_link_libraries(shgui-example PUBLIC 
    shgui shgui-app shvulkan glfw
)
set_target_properties(shgui-example shgui-app shvulkan glfw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
endfunction()



#OPTIONS
option(SH_GUI_BUILD_EXAMPLE OFF)



function(main)
build_shgui()
build_export_shaders()
build_shgui_app()

if(SH_GUI_BUILD_EXAMPLE)
build_shgui_example()
endif(SH_GUI_BUILD_EXAMPLE)

endfunction()



main()