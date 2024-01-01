
# Linux build logs

![](linux-exit-code.svg)

```
system        : Linux
version       : #1 SMP Thu Oct 5 21:02:42 UTC 2023
platform      : Linux-5.15.133.1-microsoft-standard-WSL2-x86_64-with-glibc2.31
processor     : 
min frequency : 0.0 MHz
max frequency : 0.0 MHz
cores         : 4
```

build ran for `80.16 s` and terminated with exit code `0`

---

```bash
function PREREQUISITES {
    sudo apt -y update
    sudo apt install -y libvulkan-dev libvulkan1
    sudo apt install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev
    sudo apt install -y libwayland-dev
    sudo apt install -y gcc cmake make libc6-dev
}

PREREQUISITES > .shci/linux/prerequisites-output.txt

```

```bash
Hit:1 http://deb.debian.org/debian bullseye InRelease
Hit:2 http://security.debian.org/debian-security bullseye-security InRelease
Hit:3 http://deb.debian.org/debian bullseye-updates InRelease
Hit:4 http://ftp.debian.org/debian bullseye-backports InRelease
Reading package lists...
Building dependency tree...
Reading state information...
39 packages can be upgraded. Run 'apt list --upgradable' to see them.
Reading package lists...
Building dependency tree...
Reading state information...
libvulkan-dev is already the newest version (1.2.162.0-1).
libvulkan1 is already the newest version (1.2.162.0-1).
0 upgraded, 0 newly installed, 0 to remove and 39 not upgraded.
Reading package lists...
Building dependency tree...
Reading state information...
libxcursor-dev is already the newest version (1:1.2.0-2).
libxi-dev is already the newest version (2:1.7.10-1).
libxinerama-dev is already the newest version (2:1.1.4-2).
libxrandr-dev is already the newest version (2:1.5.1-1).
libx11-dev is already the newest version (2:1.7.2-1+deb11u2).
0 upgraded, 0 newly installed, 0 to remove and 39 not upgraded.
Reading package lists...
Building dependency tree...
Reading state information...
libwayland-dev is already the newest version (1.18.0-2~exp1.1).
0 upgraded, 0 newly installed, 0 to remove and 39 not upgraded.
Reading package lists...
Building dependency tree...
Reading state information...
cmake is already the newest version (3.18.4-2+deb11u1).
gcc is already the newest version (4:10.2.1-1).
make is already the newest version (4.3-4.1).
libc6-dev is already the newest version (2.31-13+deb11u7).
0 upgraded, 0 newly installed, 0 to remove and 39 not upgraded.

```

---
    
```bash
function BUILD {
	mkdir build
    cd    build
	mkdir linux
    cd    linux

    cmake ../../ -DSH_GUI_BUILD_EXAMPLE=ON
    cmake --build .
}

BUILD > .shci/linux/build-output.txt

```

```bash
-- The C compiler identification is GNU 10.2.1
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Found Vulkan: /usr/lib/x86_64-linux-gnu/libvulkan.so  
-- shvulkan message: found Vulkan
-- shvulkan message: Vulkan_INCLUDE_DIR: /usr/include
-- shvulkan message: Vulkan_LIBRARY:     /usr/lib/x86_64-linux-gnu/libvulkan.so
-- SH_GUI_VERSION       : pre-release
-- SH_GUI_ROOT_DIR      : /mnt/d/home/desktop/github/shgui
-- SH_GUI_BINARIES_DIR  : /mnt/d/home/desktop/github/shgui/bin
-- SH_GUI_BUILD_EXAMPLE : ON
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE  
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
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
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/d/home/desktop/github/shgui/build/linux
Scanning dependencies of target shvulkan
[  2%] Building C object externals/shvulkan/CMakeFiles/shvulkan.dir/shvulkan/src/shvulkan/shVulkan.c.o
[  5%] Linking C static library ../../../../bin/linux/libshvulkan.a
[  5%] Built target shvulkan
Scanning dependencies of target shgui
[  8%] Building C object CMakeFiles/shgui.dir/shgui/src/shgui.c.o
[ 10%] Building C object CMakeFiles/shgui.dir/shgui/src/shgui-font.c.o
[ 13%] Linking C static library ../../bin/linux/libshgui.a
[ 13%] Built target shgui
Scanning dependencies of target glfw
[ 16%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/context.c.o
[ 18%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/init.c.o
[ 21%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/input.c.o
[ 24%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/monitor.c.o
[ 27%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/platform.c.o
[ 29%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/vulkan.c.o
[ 32%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/window.c.o
[ 35%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/egl_context.c.o
[ 37%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/osmesa_context.c.o
[ 40%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_init.c.o
[ 43%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_monitor.c.o
[ 45%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_window.c.o
[ 48%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/null_joystick.c.o
[ 51%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_module.c.o
[ 54%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_time.c.o
[ 56%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_thread.c.o
[ 59%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/x11_init.c.o
[ 62%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/x11_monitor.c.o
[ 64%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/x11_window.c.o
[ 67%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/xkb_unicode.c.o
[ 70%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/glx_context.c.o
[ 72%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/linux_joystick.c.o
[ 75%] Building C object externals/glfw/src/CMakeFiles/glfw.dir/posix_poll.c.o
[ 78%] Linking C static library ../../../../../bin/linux/libglfw3.a
[ 78%] Built target glfw
Scanning dependencies of target shgui-app
[ 81%] Building C object CMakeFiles/shgui-app.dir/shgui-app/src/shgui-app.c.o
[ 83%] Linking C static library ../../bin/linux/libshgui-app.a
[ 83%] Built target shgui-app
Scanning dependencies of target shgui-example
[ 86%] Building C object CMakeFiles/shgui-example.dir/shgui-example/src/shgui-example.c.o
[ 89%] Linking C executable ../../bin/linux/shgui-example
[ 89%] Built target shgui-example
Scanning dependencies of target native-export
[ 91%] Building C object externals/native-export/c/CMakeFiles/native-export.dir/native-export/src/native-export.c.o
[ 94%] Linking C static library ../../../../../bin/linux/libnative-export.a
[ 94%] Built target native-export
Scanning dependencies of target shgui-export-shaders
[ 97%] Building C object CMakeFiles/shgui-export-shaders.dir/shgui-shaders/src/export-shaders.c.o
[100%] Linking C executable ../../bin/linux/shgui-export-shaders
[100%] Built target shgui-export-shaders

```

---
    
build ran for `80.16 s` and terminated with exit code `0`

---

