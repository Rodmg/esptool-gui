# esptool-gui

A GUI for ESP8266 flash tool esptool-ck with basic features, geared towards being a production flashing tool.

<img src="esptool-gui-screenshot.png?raw=true" alt="esptool-gui screenshot" width="500px">

## Installing

You can download prebuilt binaries from the [releases section](https://github.com/Rodmg/esptool-gui/releases).

## Building

**Note:** Currently using esptool-ck 0.4.8

### OSX

1. You should have Qt5 and QtCreator developer tools installed in your system (https://www.qt.io/developers/). tested with Qt 5.6.0 and Qt Creator 4.0.0.
2. Clone this repo and open ``esptool-gui.pro`` with Qt Creator, build from there.
3. For deploying the application, you should include the Qt libraries in te app bundle, you can use the mac deployment tool, more info here: http://doc.qt.io/qt-5/osx-deployment.html

### Windows

1. You should have Qt5 and QtCreator developer tools installed in your system (https://www.qt.io/developers/). tested with Qt 5.7.0 and Qt Creator 4.0.1.
2. Clone this repo and open ``esptool-gui.pro`` with Qt Creator, build from there.
3. Copy ``tool-esptool`` folder from ``tools-windows`` to the folder where the compiled exe is.
4. For deploying the application, you should include the Qt libraries in te app folder: http://doc.qt.io/qt-5/windows-deployment.html

### Linux and other systems

I haven't tested this in Linux, however building should be more or less straight forward. The only special build step that should change is the part where the [esptool-ck binary](https://github.com/igrr/esptool-ck/releases) is bundled, you should bundle the correct version for your operating system.