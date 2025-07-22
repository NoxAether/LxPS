# Port Station

LxPS allows users to interface directly with serial ports on linux systems (for now).

The main goal of this tool is to enhance interfacing with hardware eg., embedded devices directly for direct hardware programming.

This is in a constant work in progress.

Deps:
    Boost
    notcurses
    sd-device (lib from systemd)

To build the project. Please under the root directory run:

mkdir build

cd build

cmake ..

make

You will find the executable in build/bin/
