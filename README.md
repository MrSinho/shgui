# shgui

## Build status
[![linux-status](.shci/linux-exit_code.svg)](.shci/linux-log.md)
[![windows-status](.shci/windows-exit_code.svg)](.shci/windows-log.md)

![screenshot](media/shgui-example-0.png)

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

## Issues:
* scaling the UI is ok
* changing the window size will mess up the menu bar

## To do:
* heap cleanup
* native export library implementation fix
* menu popup
* minimize window
* input field
* color picker
* graph
* loading bar
* color matrix
* texture loader
* font customization (too much for me)
