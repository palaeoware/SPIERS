# -------------------------------------------------
# Project created by QtCreator 2009-02-17T13:01:48
# -------------------------------------------------
QT += opengl
TARGET = SPIERSview2
TEMPLATE = app
RESOURCES = sview.qrc
ICON = sviewicon.icns
SOURCES += main.cpp \
    mainwindow.cpp \
    SPIERSviewglobals.cpp \
    spvreader.cpp \
    svobject.cpp \
    spv.cpp \
    compressedslice.cpp \
    mc.cpp \
    svglwidget.cpp \
    myconnectivityfilter.cpp \
    movetogroup.cpp
HEADERS += mainwindow.h \
    SPIERSviewglobals.h \
    spvreader.h \
    svobject.h \
    spv.h \
    compressedslice.h \
    mc.h \
    svglwidget.h \
    myconnectivityfilter.h \
    movetogroup.h \
    main.h
FORMS += mainwindow.ui \
    movetogroup.ui
LIBS += -L/Users/dereksiveter/SPIERS/VTK-bin/lib/vtk-5.2 \
    -lvtkRendering \
    -lvtkGraphics \
    -lvtkImaging \
    -lvtkIO \
    -lvtkFiltering \
    -lvtkCommon \
    -lvtkftgl \
    -lvtkfreetype \
    -lvtkDICOMParser \
    -lvtkpng \
    -lvtktiff \
    -lvtkzlib \
    -lvtkjpeg \
    -lvtkexpat \
    -lvtksys \
    -lstdc++ \
    -mlibz.a
INCLUDEPATH += "/Users/dereksiveter/SPIERS/VTK-bin/include/vtk-5.2"
