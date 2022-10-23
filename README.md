# shgui

## Build status
[![linux-status](.shci/linux-status.svg)](.shci/linux-log.md)
[![windows-status](.shci/windows-status.svg)](.shci/windows-log.md)

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
cmake .. -DSH_GUI_BUILD_EXAMPLE=ON
cmake --build .
```

## Binaries and output 
The compiled binaries are located at the [`bin`](/bin) directory.

## To do:
* round char size to 7* 7* 3 bytes
* menu popup
* minimize window
* input field
* slider
* color picker
* graph
* font customization

 # Opening issues and contributing

Yesn't
