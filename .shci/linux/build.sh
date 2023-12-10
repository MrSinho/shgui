function BUILD {
	mkdir build
	mkdir build/linux
    cd build/linux

    cmake ../../ -DSH_GUI_BUILD_EXAMPLE=ON
    cmake --build .
}

BUILD > .shci/linux/build-output.txt
