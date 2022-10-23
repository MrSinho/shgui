
# shgui

![windows-badge](windows-exit_code.svg)

## [windows build logs:](https://github.com/mrsinho/shci)

        

Build ran for `42.47s`

---

```bash
choco install -y mingw cmake
mingw-get install gcc mingw32-make gdb
```

```bash
Chocolatey v0.11.2
Installing the following packages:
mingw;cmake
By installing, you accept licenses for the packages.
mingw v11.2.0.07112021 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.
cmake v3.24.2 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.

Chocolatey installed 0/2 packages. 
 See the log for details (C:\ProgramData\chocolatey\logs\chocolatey.log).

Warnings:
 - cmake - cmake v3.24.2 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.
 - mingw - mingw v11.2.0.07112021 already installed.
 Use --force to reinstall, specify a version to install, or try upgrade.

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
-- Building for: Visual Studio 17 2022
-- Selecting Windows SDK version 10.0.22621.0 to target Windows 10.0.21996.
-- The C compiler identification is MSVC 19.32.31332.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.32.31326/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Found Vulkan: C:/VulkanSDK/1.2.198.1/Lib/vulkan-1.lib (found version "1.2.198") found components: glslc glslangValidator 
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Failed
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - not found
-- Found Threads: TRUE  
-- Found Doxygen: C:/ProgramData/chocolatey/bin/doxygen.exe (found version "1.9.1") found components: doxygen 
-- Including Win32 support
-- Configuring done
-- Generating done
-- Build files have been written to: C:/Users/sino3/Desktop/shci-test/shgui/shgui/build
Microsoft (R) Build Engine version 17.2.1+52cd2da31 for .NET Framework
Copyright (C) Microsoft Corporation. All rights reserved.

  Checking Build System
  Generating HTML documentation
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/externals/glfw/docs/CMakeLists.txt
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/externals/native-export/c/CMakeLists.txt
  native-export.c
  native-export.vcxproj -> C:\Users\sino3\Desktop\shci-test\shgui\shgui\bin\Debug\native-export.lib
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/CMakeLists.txt
  export-shaders.c
  export-shaders.vcxproj -> C:\Users\sino3\Desktop\shci-test\shgui\shgui\build\Debug\export-shaders.exe
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/externals/glfw/src/CMakeLists.txt
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
  Generating Code...
  Compiling...
  wgl_context.c
  Generating Code...
  glfw.vcxproj -> C:\Users\sino3\Desktop\shci-test\shgui\shgui\bin\Debug\glfw3.lib
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/externals/shvulkan/CMakeLists.txt
  shVkCore.c
  shVkMemoryInfo.c
  shVkPipelineData.c
  shVkCheck.c
  shVkDrawLoop.c
  Generating Code...
  shvulkan.vcxproj -> C:\Users\sino3\Desktop\shci-test\shgui\shgui\bin\Debug\shvulkan.lib
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/CMakeLists.txt
  shgui.c
  shgui.vcxproj -> C:\Users\sino3\Desktop\shci-test\shgui\shgui\bin\Debug\shgui.lib
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/CMakeLists.txt
  shgui-example.c
  shgui-example.vcxproj -> C:\Users\sino3\Desktop\shci-test\shgui\shgui\bin\Debug\shgui-example.exe
  Building Custom Rule C:/Users/sino3/Desktop/shci-test/shgui/shgui/CMakeLists.txt

```

---

    

Build terminated with exit code 0

---

