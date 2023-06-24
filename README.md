# shgui

### DO NOT DOWNLOAD IT'S CURRENTLY BUGGED AF, I SHOULD DEBUG THE SOURCE CODE IN ORDER TO FIX THE LIBRARY BUT I DON'T THINK I'M EVER GOING TO DO SO.

## Build status
[![linux-status](.shci/linux-exit_code.svg)](.shci/linux-log.md)
[![windows-status](.shci/windows-exit_code.svg)](.shci/windows-log.md)

![screenshot](media/shgui-example-1.png)

## Features:
* Widgets
    * menu bar
        * menu item
    * window
        * highlight edge
        * movable
        * input conditions
        * change cursor default icons
    * text
    * button
    * popup

## Build from source
```batch
cd shgui
mkdir build
cd build
cmake .. #-DSH_GUI_BUILD_EXAMPLE=ON
cmake --build .
```

## Binaries and output 
The compiled binaries are located at the [`bin`](/bin) directory.

## Bugs:
* Popup macro system is working, but clicking on the new region messes up priority values

## To do:
* input field --> listen for key inputs, render chars if input field is selected, return string on confirmation
* shGuiRegionColorMatrix / shGuiWindowColorMatrix 
* graphs
* minimize window
* loading bar
* texture loader
* checkbox
* font customization (too much for me, i'm fine with my pixel characters)
