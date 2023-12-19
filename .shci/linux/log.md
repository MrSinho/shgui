
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

build ran for `94.37 s` and terminated with exit code `0`

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
Hit:2 http://deb.debian.org/debian bullseye-updates InRelease
Hit:3 http://security.debian.org/debian-security bullseye-security InRelease
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
	mkdir build/linux
    cd build/linux

    cmake ../../ -DSH_GUI_BUILD_EXAMPLE=ON
    cmake --build .
}

BUILD > .shci/linux/build-output.txt

```

```bash
-- shvulkan message: found Vulkan
-- shvulkan message: Vulkan_INCLUDE_DIR: /usr/include
-- shvulkan message: Vulkan_LIBRARY:     /usr/lib/x86_64-linux-gnu/libvulkan.so
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Including X11 support
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Configuring done
-- Generating done
-- Build files have been written to: /mnt/d/home/desktop/github/shgui/build/linux
[  5%] Built target shvulkan
[ 13%] Built target shgui
[ 78%] Built target glfw
[ 83%] Built target shgui-app
[ 89%] Built target shgui-example
[ 94%] Built target native-export
[100%] Built target shgui-export-shaders

```

---
    
build ran for `94.37 s` and terminated with exit code `0`

---

