# SPIERS Software

Coded by:

 - Mark Sutton (m.sutton@imperial.ac.uk)
 - Russell J. Garwood (russell.garwood@manchester.ac.uk)
 - Alan R.T. Spencer (alan.spencer@imperial.ac.uk)

<p align="center">
  <img width="250" height="250" src="./resources/palaeoware_logo_square.png">
</p>

______

## Relevant references:
<b>Sutton, M.D., Garwood, R.J., Siveter, D.J. &amp; Siveter, D.J.</b> 2012. Spiers and VAXML; A software toolkit for tomographic visualisation, and a format for virtual specimen interchange. <a href="http://palaeo-electronica.org/content/issue-2-2012-technical-articles/226-virtual-palaeontology-toolkit"><i>Palaeontologia Electronica</i> 15(2): 15.2.5T</a>

______

SPIERS (Serial Palaeontological Image Editing and Rendering System) is a package of three programs for the reconstruction and analysis of tomographic (serial image) datasets, such as those obtained from serial-grinding of specimens, or from CT scanning. More details are available in the documentation for each program.
______

## Compiling from source:

### Windows 64-bit - QT Creator + QT v5.x and VTK v8.1 using MSYS2 (64-bit) and MinGW (64-bit)
We recommend you install and use MSYS2 (64-bit) a Windows package manager, based on modern Cygwin (POSIX compatibility layer) and MinGW-w64, that allows easy installation of QT v5.x 64-bit and VTK v8.1.

1. Dowload and run the latest version of MSYS2 https://www.msys2.org/ for 64-bit Windows. This will be name "mysys2-x86_64-..." for the 64-bit installer.
2. Follow the install instructions. We have used the default install location of "C:\mysys64\" and it is here that includes required in the .pro files point. If you install MSYS2 to another location the .pro files will need to be updated to your install location.
3. Once installed open up MSYS2 shell and run the pacman update command: pacman -Syu Note that as this will almost certainly update pacman itself you may have to close down and restart the MYSYS2 shell before re-running the command to finish.
4. Once MSYS2 and pacman are fully updated run the following comand to install QT 5.x and it's dependancies: pacman -S mingw-w64-x86_64-qt-creator mingw-w64-x86_64-qt5
5. (OPTIONAL) If you intend on debugging the software in QT and wish to use GDB then run the following to install the matching GBD debugger: pacman -S mingw-w64-x86_64-gdb
6. At this stage you should have the following under the MYSYS2 install location:
- {install location}/mingw64 (Main ming64 folder)
- {install location}/mingw64/bin/qmake.exe (QMake for QT version)
- {install location}/mingw64/bin/g++.exe (C++ complier)
- {install location}/mingw64/bin/gcc.exe (C complier)
- {install location}/mingw64/bin/gdb.exe (Debugger | OPTIONAL)
7. We now want to install the VTK v8.1 package. To do this run the following command: pacman -S mingw-w64-x86_64-vtk
8. You should now be able to find the required libaries under "{install location}/mingw64/bin" and the required header (.h) files for QT v5.x and VTK v8.1 under "{install location}/mingw64/include".
9. Use the information above to setup a new 64-bit ming64 kit under QT creator and follow standard QT Creator debug/release procedure.
