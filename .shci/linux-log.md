
# shgui

![linux-badge](linux-exit_code.svg)

## [linux build logs:](https://github.com/mrsinho/shci)

        

Build ran for `63.33s`

---

```bash
sudo apt install -y gcc cmake make gdb libc6-dev
```

```bash
Reading package lists...
Building dependency tree...
Reading state information...
cmake is already the newest version (3.18.4-2+deb11u1).
gcc is already the newest version (4:10.2.1-1).
gdb is already the newest version (10.1-1.7).
make is already the newest version (4.3-4.1).
libc6-dev is already the newest version (2.31-13+deb11u5).
0 upgraded, 0 newly installed, 0 to remove and 11 not upgraded.

```

---

    
```bash
cd shgui
mkdir build
cd build
cmake .. -DSH_GUI_BUILD_EXAMPLE=ON
cmake --build .
```

```bash
-- The C compiler identification is GNU 10.2.1
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Found Vulkan: /usr/lib/x86_64-linux-gnu/libvulkan.so  
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE  
-- Found Doxygen: /usr/bin/doxygen (found version "1.9.1") found components: doxygen 
-- Including X11 support
-- Found X11: /usr/include   
-- Looking for XOpenDisplay in /usr/lib/x86_64-linux-gnu/libX11.so;/usr/lib/x86_64-linux-gnu/libXext.so
-- Looking for XOpenDisplay in /usr/lib/x86_64-linux-gnu/libX11.so;/usr/lib/x86_64-linux-gnu/libXext.so - found
-- Looking for gethostbyname
-- Looking for gethostbyname - found
-- Looking for connect
-- Looking for connect - found
-- Looking for remove
-- Looking for remove - found
-- Looking for shmat
-- Looking for shmat - found
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/c/Users/sino3/Desktop/shci-unix-test/shgui/shgui/build
Scanning dependencies of target shvulkan
[  2%] Building C object externals/shvulkan/CMakeFiles/shvulkan.dir/shvulkan/src/shvulkan/shVkCore.c.o
[  5%] Building C object externals/shvulkan/CMakeFiles/shvulkan.dir/shvulkan/src/shvulkan/shVkMemoryInfo.c.o
[  7%] Building C object externals/shvulkan/CMakeFiles/shvulkan.dir/shvulkan/src/shvulkan/shVkPipelineData.c.o
[ 10%] Building C object externals/shvulkan/CMakeFiles/shvulkan.dir/shvulkan/src/shvulkan/shVkCheck.c.o
[ 12%] Building C object externals/shvulkan/CMakeFiles/shvulkan.dir/shvulkan/src/shvulkan/shVkDrawLoop.c.o
[ 15%] Linking C static library ../../../bin/libshvulkan.a
[ 15%] Built target shvulkan
Scanning dependencies of target shgui
[ 17%] Building C object CMakeFiles/shgui.dir/shgui/src/shgui.c.o
[ 20%] Linking C static library ../bin/libshgui.a
[ 20%] Built target shgui
Scanning dependencies of target glfw
[ 23%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/context.c.o
[ 25%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/init.c.o
[ 28%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/input.c.o
[ 30%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/monitor.c.o
[ 33%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/platform.c.o
[ 35%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/vulkan.c.o
[ 38%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/window.c.o
[ 41%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/egl_context.c.o
[ 43%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/osmesa_context.c.o
[ 46%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_init.c.o
[ 48%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_monitor.c.o
[ 51%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_window.c.o
[ 53%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_joystick.c.o
[ 56%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_module.c.o
[ 58%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_time.c.o
[ 61%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_thread.c.o
[ 64%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/x11_init.c.o
[ 66%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/x11_monitor.c.o
[ 69%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/x11_window.c.o
[ 71%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/xkb_unicode.c.o
[ 74%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/glx_context.c.o
[ 76%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/linux_joystick.c.o
[ 79%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_poll.c.o
[ 82%] Linking C static library ../../../../bin/libglfw3.a
[ 82%] Built target glfw
Scanning dependencies of target shgui-example
[ 84%] Building C object CMakeFiles/shgui-example.dir/shgui-example/src/shgui-example.c.o
[ 87%] Linking C executable ../bin/shgui-example
[ 87%] Built target shgui-example
Scanning dependencies of target native-export
[ 89%] Building C object externals/native-export/c/CMakeFiles/native-export.dir/native-export/src/native-export.c.o
[ 92%] Linking C static library ../../../../bin/libnative-export.a
[ 92%] Built target native-export
Scanning dependencies of target export-shaders
[ 94%] Building C object CMakeFiles/export-shaders.dir/shaders/export-shaders.c.o
[ 97%] Linking C executable export-shaders
[ 97%] Built target export-shaders
Scanning dependencies of target docs
[100%] Generating HTML documentation
[100%] Built target docs

```

---

    

Build terminated with exit code 0

---

