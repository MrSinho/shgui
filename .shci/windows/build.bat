> .shci/windows/build-output.txt (
    mkdir build
    mkdir build/windows
    cd build/windows
    
    cmake -DSH_GUI_BUILD_EXAMPLE=ON ..
    cmake --build .
)
