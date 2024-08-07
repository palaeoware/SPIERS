# SPIERS Software

Coded by:

 - Mark Sutton (m.sutton@imperial.ac.uk)
 - Russell J. Garwood (russell.garwood@gmail.com)
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk)

<p align="center">
  <img width="250" height="250" src="./resources/palaeoware_logo_square.png">
</p>

______

## Relevant references:
<b>Sutton, M.D., Garwood, R.J., Siveter, D.J. &amp; Siveter, D.J.</b> 2012. Spiers and VAXML; A software toolkit for tomographic visualisation, and a format for virtual specimen interchange. <a href="http://palaeo-electronica.org/content/issue-2-2012-technical-articles/226-virtual-palaeontology-toolkit"><i>Palaeontologia Electronica</i> 15(2): 15.2.5T</a>

_____


CONTENTS:

1. Copyright and Licence
2. Introduction
3. Installation
4. Minimum Requirements
5. Documentation
6. Contributing to the code
7. Bug reporting and feature requests

_____

## 1. Copyright and Licence

All SPIERS code is released under the GNU General Public License. See LICENSE.md files in the programme directories.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but **without any warranty**.

_____

## 2. Introduction

SPIERS (Serial Palaeontological Image Editing and Rendering System) is a package of three programs for the reconstruction and analysis of tomographic (serial image) datasets, such as those obtained from serial-grinding of specimens, or from CT scanning. More details are available in the documentation for each program.
______

## 3. Installation

SPIERS may be compiled using QT from the source code or installed from the binary releases on our [GitHub](https://github.com/palaeoware).

### Compiling from source

#### Windows 64-bit - QT Creator + QT v5.x and VTK v8.1 using MSYS2 (64-bit) and MinGW (64-bit)
We recommend you install and use MSYS2 (64-bit) a Windows package manager, based on modern Cygwin (POSIX compatibility layer) and MinGW-w64, that allows easy installation of QT v5.x 64-bit and VTK v8.1.

1. Download and run the latest version of MSYS2 https://www.msys2.org/ for 64-bit Windows. This will be name "mysys2-x86_64-..." for the 64-bit installer.
2. Follow the install instructions. We have used the default install location of "C:\mysys64\" and it is here that includes required in the .pro files point. If you install MSYS2 to another location the .pro files will need to be updated to your install location.
3. Once installed open up MSYS2 shell and run the pacman update command: pacman -Syu Note that as this will almost certainly update pacman itself you may have to close down and restart the MYSYS2 shell before re-running the command to finish.
4. Once MSYS2 and pacman are fully updated run the following command to install QT 5.x and its dependencies: pacman -S mingw-w64-x86_64-qt-creator mingw-w64-x86_64-qt5
5. (OPTIONAL) If you intend on debugging the software in QT and wish to use GDB then run the following to install the matching GBD debugger: pacman -S mingw-w64-x86_64-gdb
6. At this stage you should have the following under the MYSYS2 install location:
- {install location}/mingw64 (Main ming64 folder)
- {install location}/mingw64/bin/qmake.exe (QMake for QT version)
- {install location}/mingw64/bin/g++.exe (C++ complier)
- {install location}/mingw64/bin/gcc.exe (C complier)
- {install location}/mingw64/bin/gdb.exe (Debugger | OPTIONAL)
7. We now want to install the VTK v8.1 package. To do this run the following command: pacman -S mingw-w64-x86_64-vtk
8. You should now be able to find the required libraries under "{install location}/mingw64/bin" and the required header (.h) files for QT v5.x and VTK v8.1 under "{install location}/mingw64/include".
9. Use the information above to setup a new 64-bit ming64 kit under QT creator and follow standard QT Creator debug/release procedure.

#### Ubuntu 18.04/20.04 64-bit - QT Creator + QT v5.x and VTK v7.1 using GCC (64-bit)

1. Install GCC and QT5.X on your system. You can do this two ways, by using system packages:

  `sudo apt-get install build-essential libgl1-mesa-dev`

  `sudo apt install qt5-default`

  Or by downloading and running the installer from Qt: https://www.qt.io/download Further instructions are available here: https://wiki.qt.io/Install_Qt_5_on_Ubuntu

2. SPIERSview requires VTK: The Linux version of SPIERSview currently uses VTK7.1. This can also be installed from the Ubuntu packages:

  `sudo apt-get install libvtk7-qt-dev`

3. Using the above package, you should be able to find the required VTK libraries under /usr/include/vtk-7.1/ They can also be included, if needed, in the working directory, and added to the executable at link time using the QMAKE_RPATHDIR variable - e.g. #QMAKE_RPATHDIR += $$PWD/vtk-7.X/

4. Download source code, or clone using Git:

  `git clone https://github.com/palaeoware/SPIERS.git`

  You can then compile SPIERS by opening the .pro file in QT creator and following standard debug/release procedure (or see alternative below). The resulting executables will be placed in their individual bin folders. SPIERS is set up to work with this directory structure. Copy the build folders to your required location if this differs from the build location.

5. Alternatively you can build using the following commands -  navigate into the SPIERS source folder in a terminal window:

  `cd SPIERS`

  Create a makefile:

  `qmake ./SPIERS.pro`

  Build by running the make command:

  `make`

  You can then copy the build folders to your desired location.

6. Should you wish to create desktop icons or for SPIERS to appear in your launcher, we provide exemplar desktop entries in the SPIERS/examples folder of the repository, coupled with icons for these. Place the icons in the same folder alongside the executable files, and place this folder wherever you would like it to live on your system. The edit the paths to the executables and icons by replacing XXXXX in the example files with your path. Double clicking on the desktop entry will ask you to trust and launch the program. Placing the entry on your desktop will create an icon that launches the software. Additionally, if you open a terminal at the location where the desktop entry is stored, and copy it from there to /usr/share/applications/ using this command:

  `sudo cp SPIERSalign.desktop /usr/share/applications/`

  `sudo cp SPIERSedit.desktop /usr/share/applications/`

  `sudo cp SPIERSview.desktop /usr/share/applications/`

  This will create entries for each package in the GNOME activities overview / launcher. Alternatively you can use alacarte to achieve this process using a GUI:

  `sudo apt-get update`
  `sudo apt-get install alacarte`

#### macOS Catalina/Big Sur 64-bit - QT Creator + QT v5.x and VTK v9.1.x (via Homebrew) using Clang 64-bit/XCode 11 toolchain

1. Install XCode 11 (for Catalina) or XCode 12 (for Big Sur) from Apple App Store.
2. Install QT5.x/QT Creator from https://www.qt.io/download.
3. Install Homebrew (https://brew.sh/).
4. Get VTK 9.1 package via Homebrew - you may need to force Homebrew to install a specific VTK version see Homebrew instruction on how to do so:

  `brew install vtk`

5. Check/Update the path to the VTK package in the ./SPIERSview/SPIERSview.pro file, where "9.1.0" is your installed version number:

'LIBS += -L/usr/local/Cellar/vtk/9.1.0/lib/'

6. You should then be able to compile SPIERS by opening the .pro file in QT creator and following standard debug/release procedure.

Note: it may be possible to install QT5.x/QT Creator via Homebrew rather than via the Apple App Store.

#### macOS Monterey 64-bit - QT Creator + QT v5.x and VTK v9.1.x (via Homebrew) using Clang 64-bit/XCode 13 toolchain

1. Install XCode 12 from Apple App Store.
2. Install QT5.x/QT Creator from https://www.qt.io/download.

##### [Apple Intel Chip - x86_64]

3. Install Homebrew (https://brew.sh/).
4. Get VTK 8.2 package via Homebrew - you may need to force Homebrew to install a specific VTK version see Homebrew instruction on how to do so:

  `brew install vtk`

##### [Apple M1 Chip - arm64]

On MacOS running on arm64 (M1 chip) homebrew must be installed under x86_64 to work with QT 5.x

3. Install Homebrew (https://brew.sh/). To do this:

  `cd ~/Downloads`

  `mkdir homebrew`

  `curl -L https://github.com/Homebrew/brew/tarball/`master | tar xz --strip 1 -C homebrew`

  `sudo mv homebrew /usr/local/homebrew`

  `export PATH=$HOME/bin:/usr/local/bin:$PATH`

  `alias brew_86='arch -x86_64 /usr/local/homebrew/bin/brew'`

  If this is not done then homebrew with get installed under arm64 architecture making the VTK libs incompatible with the x86_64 QT compile that is installed by default from the QT binary. QT 6.2 claims to be a universal build (i.e. x86_64 + arm64) but VTK cannot currently be build as a universal library. As such on arm64 MacOS machines we are targeting the Intel x86_64 so that it runs on older Macs and under Rosetta on arm64 (M1 chip) machines. Given more time both QT and VTK might both catch up with the newer Apple silicon around.

4. Get VTK 9.1 package via Homebrew - you may need to force Homebrew to install a specific VTK version see Homebrew instruction on how to do so:

  `brew_86 insall vtk`

5. Check/Update the path to the VTK package in the ./SPIERSview/SPIERSview.pro file, where "9.1.0" is your installed version number:

  'LIBS += -L/usr/local/Cellar/vtk/9.1.0/lib/'

6. You should then be able to compile SPIERS by opening the .pro file in QT creator and following standard debug/release procedure.

7. Packaging of the individual apps is achieved by using the QT macdeployqt program:

  `/{path to QT}/Qt/5.15.2/clang_64/bin/macdeployqt /{path to build}/build-SPIERS-Desktop_Qt_5_15_2_clang_64bit-Release/SPIERSalign/bin/SPIERSalign64.app -always-overwrite -verbose=2`

  `/{path to QT}/Qt/5.15.2/clang_64/bin/macdeployqt /{path to build}/build-SPIERS-Desktop_Qt_5_15_2_clang_64bit-Release/SPIERSedit/bin/SPIERSedit64.app -always-overwrite -verbose=2`

  `/{path to QT}/Qt/5.15.2/clang_64/bin/macdeployqt /{path to build}/build-SPIERS-Desktop_Qt_5_15_2_clang_64bit-Release/SPIERSview/bin/SPIERSview64.app -always-overwrite -verbose=2`

Note: it may be possible to install QT5.x/QT Creator via Homebrew rather than via the Apple App Store.
_____

## 4. Minimum Requirements

SPIERS is designed to run on a wide range of hardware, rather than being restricted to workstations. The exact requirements for use depend on the data being processed.

_____

## 5. Documentation

All documentation for SPIERS is online at Read the Docs. Links are found on the [SPIERS website](https://spiers-software.org/). This is a version controlled user manual automatically parsed from the .rst found under the docs folders in our software folders.

_____

## 6. Contributing to the code

Contribution to the code in this repository is welcome. Please read the [Repository Contribution](https://github.com/palaeoware/repoconventions) document for information on our licencing, code of conduct, coding style, and repository structure before submitting any code updates.

_____


## 7. Bug reporting and feature requests

Bug reporting and feature requests should be made through the [GitHub Issues](../../issues) page for this repository. We will aim to respond to all issues and feature requests in a timely manner.

_____

w:spiers-software.org/

t:@palaeoware

e:palaeoware@gmail.com

w:https://github.com/palaeoware
