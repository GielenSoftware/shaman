# Shaman

## Description

Shaman is a software that can communicate with the low-level spirits of libsensors and display their messages in an easy-to-read Graphic Interface.

## Dependencies and compilation

Shaman uses GTK4 and libsensors libraries. Make sure those are installed correctly before attempting to compile the source code.
Compilation can be made with Make and the Makefile provided : 

    $ make -f Makefile shaman

## Configuration and software limitation

Shaman uses the same config file as sensors, the data displayed should therefore be similar. 
However, Shaman currently only support 4 types of sensors : FAN, TEMP, POWER and CURRENT.
Shaman can currently only display INPUT type values (the current value return by a sensor).
Shaman also assumes that the values of TEMP sensors types are output in °C.
Support for other sensors and value types will be added in future versions.

## Use

At start-up, Shaman will only display a static view of sensors reading. 
To start updating those value in real-time, click on "Start Monitoring" in the menu or use the CTRL+m keyboard shortcut.
To stop monitoring, click on "Stop Monitoring" in the menu or use the CTRL+p keyboard shortcut.

## License and copyright

This code is published under the GNU GPL 3 license. 
The copyright holder is Julien Gielen.