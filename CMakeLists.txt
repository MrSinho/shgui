cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

project(shgui C)

#SUBDIRECTORIES
find_package(Vulkan REQUIRED FATAL_ERROR)


function(build_shgui)
add_subdirectory(externals/shvulkan)
add_library(shgui 
    shgui/src/shgui.c
)
target_include_directories(shgui PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/shgui/include
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts/include
)
target_link_libraries(shgui PRIVATE shvulkan)
set_target_properties(shgui shvulkan PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
endfunction()



function(build_shgui_example)
add_subdirectory(externals/glfw)
add_executable(shgui-example 
    shgui-example/src/shgui-example.c
)
target_link_libraries(shgui-example PRIVATE 
    shgui shvulkan glfw
)
set_target_properties(shgui-example shvulkan glfw PROPERTIES 
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
endfunction()


#OPTIONS
option(SH_GUI_BUILD_EXAMPLE OFF)

function(main)
build_shgui()

if(SH_GUI_BUILD_EXAMPLE)
build_shgui_example()
endif(SH_GUI_BUILD_EXAMPLE)

endfunction()



main()