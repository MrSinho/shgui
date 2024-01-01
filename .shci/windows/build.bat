> .shci/windows/build-output.txt (
    mkdir build
    cd    build
    mkdir windows
    cd    windows
    
    cmake ../../ -DSH_GUI_BUILD_EXAMPLE=ON
    cmake --build .
)
