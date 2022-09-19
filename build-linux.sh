#!/usr/bin/env bash

set -euo pipefail
IFS=$'\n\t'

# Make sure to have the required qt libraries installed
# sudo apt install build-essential qtbase5-dev libqt5serialport5-dev

rm -f esptool-gui
rm -f *.o

qmake
make

rm -rf tool-esptool
cp -r tools-linux/tool-esptool .
