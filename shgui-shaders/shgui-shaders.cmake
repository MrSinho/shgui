cmake_minimum_required(VERSION 3.0)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)



function(build_shgui_shaders)

if (WIN32)
add_custom_target(shgui-shaders
    COMMAND start ${SH_GUI_ROOT_DIR}/shgui-shaders/build-all.cmd
    COMMENT "shgui: building shaders"
)
else()
add_custom_target(shgui-shaders
    COMMAND cp ${SH_GUI_ROOT_DIR}/shgui-shaders/build-all.cmd ${SH_GUI_ROOT_DIR}/shgui-shaders/build-all.sh && ./${SH_GUI_ROOT_DIR}/shgui-shaders/build-all.sh
    COMMENT "shgui: building shaders"
)
endif(WIN32)

endfunction()