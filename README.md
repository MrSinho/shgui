# shgui 

![](https://img.shields.io/badge/Sinho_softworks-FF0080?style=for-the-badge&logo=&logoColor=white&labelColor=990042)
[![](https://img.shields.io/badge/GitHub_repository-000000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/mrsinho/shgui)

[TOC]

## Overview

`shgui` is a very simple and lightweight ui system for `Vulkan` applications written completely in C.

It is designed to facilitate the creation and management of basic graphical user interface elements in applications. It offers a straightforward set of functions and structures to simplify GUI development within Vulkan-based projects.

![](/.shci/linux/linux-exit-code.svg)
![](/.shci/windows/windows-exit-code.svg)

![](https://img.shields.io/badge/Written_in_C-FF0080?style=for-the-badge&logo=c&logoColor=white&labelColor=990042#.svg)
![](https://img.shields.io/badge/Compatible_with_C%2b%2b-FF0080?style=for-the-badge&logo=c%2b%2b&logoColor=white&labelColor=990042#.svg)

## Available widgets

Developers can use the shgui library to quickly incorporate basic gui elements, such as text rendering, into their c-based applications. It simplifies the process of displaying windows, characters and text on graphical surfaces, making it ideal for projects requiring simple user interface components without complex dependencies.

| Widget   | Rendering | "Features"                                                              |
|----------|-----------|-------------------------------------------------------------------------|
| `region` | Batch     | Fill color, edge color, absolute (pixel) position and relative position |
| `text`   | Batch     | Pixel rendering, fill color, font included in header, absolute position |

Feel free to check out the demo to get a better understanding. You're welcome to explore the demo by visiting it [here](/docs/src/demo.md).

## Input system

In program (missing: `shterminal`).

## Build from source

```shell
cd shgui
mkdir build
cd build
cmake .. #-DSH_GUI_BUILD_EXAMPLE=ON
cmake --build .
```

## Binaries and output 
The compiled binaries can be conveniently found in the `bin` directory.

## Contributing and support

Should you encounter any issues with the library and tools or wish to introduce new features, I welcome your contributions to the repository. You can contribute by either opening an issue or submitting a pull request. 

Should you find value in the provided resources or have enjoyed using the project, your support in the form of a tip would be greatly appreciated. It goes a long way in sustaining and enhancing the development efforts. Thank you for considering!

[![](https://img.shields.io/badge/Buy_Me_A_Coffee-FFDD00?style=for-the-badge&logo=buy-me-a-coffee&logoColor=black)](https://www.buymeacoffee.com/mrsinho)

![](https://img.shields.io/badge/Sinho_softworks-FF0080?style=for-the-badge&logo=&logoColor=white&labelColor=990042)