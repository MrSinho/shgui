
# Windows build logs

![](windows-exit-code.svg)

```
system        : Windows
version       : 10.0.22621
platform      : Windows-10-10.0.22621-SP0
processor     : AMD64 Family 23 Model 17 Stepping 0, AuthenticAMD
min frequency : 0.0 MHz
max frequency : 3500.0 MHz
cores         : 4
```

build ran for `60.46 s` and terminated with exit code `0`

---

```bash
> .shci/windows/prerequisites-output.txt (
    choco install -y curl mingw cmake
    mingw-get install gcc mingw32-make gdb
)
```

```bash
Chocolatey v2.0.0
Chocolatey detected you are not running from an elevated command shell
 (cmd/powershell).

 You may experience errors - many functions/packages
 require admin rights. Only advanced users should run choco w/out an
 elevated shell. When you open the command shell, you should ensure
 that you do so with "Run as Administrator" selected. If you are
 attempting to use Chocolatey in a non-administrator setting, you
 must select a different location other than the default install
 location. See
 https://docs.chocolatey.org/en-us/choco/setup#non-administrative-install
 for details.

For the question below, you have 20 seconds to make a selection.

 Do you want to continue?([Y]es/[N]o): 
Timeout or your choice of '' is not a valid selection.
Installing the following packages:
curl;mingw;cmake
By installing, you accept licenses for the packages.
curl v8.1.2 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.
mingw v12.2.0.03042023 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.
cmake v3.26.4 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.

Chocolatey installed 0/3 packages. 
 See the log for details (C:\ProgramData\chocolatey\logs\chocolatey.log).

Warnings:
 - cmake - cmake v3.26.4 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.
 - curl - curl v8.1.2 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.
 - mingw - mingw v12.2.0.03042023 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.

```

---
    
```bash
> .shci/windows/build-output.txt (
    mkdir build
    cd    build
    mkdir windows
    cd    windows
    
    cmake ../../ -DSH_GUI_BUILD_EXAMPLE=ON
    cmake --build .
)

```

```bash
-- Building for: Visual Studio 17 2022
-- Selecting Windows SDK version 10.0.22000.0 to target Windows 10.0.22621.
-- The C compiler identification is MSVC 19.37.32822.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: D:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Found Vulkan: D:/lib/vulkan-sdk-1.3.261.1/Lib/vulkan-1.lib (found version "1.3.261") found components: glslc glslangValidator 
-- shvulkan message: found Vulkan
-- shvulkan message: Vulkan_INCLUDE_DIR: D:/lib/vulkan-sdk-1.3.261.1/Include
-- shvulkan message: Vulkan_LIBRARY:     D:/lib/vulkan-sdk-1.3.261.1/Lib/vulkan-1.lib
-- SH_GUI_VERSION       : pre-release
-- SH_GUI_ROOT_DIR      : D:/home/desktop/github/shgui
-- SH_GUI_BINARIES_DIR  : D:/home/desktop/github/shgui/bin
-- SH_GUI_BUILD_EXAMPLE : ON
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - not found
-- Found Threads: TRUE  
-- Found Doxygen: D:/bin/doxygen/bin/doxygen.exe (found version "1.9.8") found components: doxygen 
-- Including Win32 support
-- Found Doxygen: D:/bin/doxygen/bin/doxygen.exe (found version "1.9.8") found components: doxygen missing components: dot
-- Configuring D:/home/desktop/github/shgui/docs/Doxyfile from D:/home/desktop/github/shgui/docs/Doxyfile.in
-- Creating custom docs command: D:/bin/doxygen/bin/doxygen.exe D:/home/desktop/github/shgui/docs/Doxyfile
-- Configuring done (11.2s)
-- Generating done (0.4s)
-- Build files have been written to: D:/home/desktop/github/shgui/build/windows
Versione di MSBuild L17.7.2+d6990bcfa per .NET Framework

  Checking Build System
  Generating HTML documentation
  Building Custom Rule D:/home/desktop/github/shgui/externals/glfw/docs/CMakeLists.txt
  Building Custom Rule D:/home/desktop/github/shgui/externals/glfw/src/CMakeLists.txt
  context.c
  init.c
  input.c
  monitor.c
  platform.c
  vulkan.c
  window.c
  egl_context.c
  osmesa_context.c
  null_init.c
  null_monitor.c
  null_window.c
  null_joystick.c
  win32_module.c
  win32_time.c
  win32_thread.c
  win32_init.c
  win32_joystick.c
  win32_monitor.c
  win32_window.c
  Generazione del codice in corso...
  Compilazione in corso...
  wgl_context.c
  Generazione del codice in corso...
  glfw.vcxproj -> D:\home\desktop\github\shgui\bin\windows\Debug\glfw3.lib
  Building Custom Rule D:/home/desktop/github/shgui/externals/native-export/c/CMakeLists.txt
  native-export.c
  native-export.vcxproj -> D:\home\desktop\github\shgui\bin\windows\Debug\native-export.lib
  Building Custom Rule D:/home/desktop/github/shgui/externals/shvulkan/CMakeLists.txt
  shVulkan.c
  shvulkan.vcxproj -> D:\home\desktop\github\shgui\bin\windows\Debug\shvulkan.lib
  Building Custom Rule D:/home/desktop/github/shgui/CMakeLists.txt
  shgui.c
  shgui-font.c
  Generazione del codice in corso...
  shgui.vcxproj -> D:\home\desktop\github\shgui\bin\windows\Debug\shgui.lib
  Building Custom Rule D:/home/desktop/github/shgui/CMakeLists.txt
  shgui-app.c
  shgui-app.vcxproj -> D:\home\desktop\github\shgui\bin\windows\Debug\shgui-app.lib
  Building Custom Rule D:/home/desktop/github/shgui/CMakeLists.txt
  shgui-example.c
  shgui-example.vcxproj -> D:\home\desktop\github\shgui\bin\windows\Debug\shgui-example.exe
  Building Custom Rule D:/home/desktop/github/shgui/CMakeLists.txt
  export-shaders.c
  shgui-export-shaders.vcxproj -> D:\home\desktop\github\shgui\bin\windows\Debug\shgui-export-shaders.exe
  Building Custom Rule D:/home/desktop/github/shgui/CMakeLists.txt

```

---
    
build ran for `60.46 s` and terminated with exit code `0`

---

