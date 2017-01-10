QT = gui core network widgets
CONFIG += qt \
    release \
    warn_on
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui \
    ui/about.ui
HEADERS = src/mainwindowimpl.h \
    src/globals.h \
    src/scene.h \
    src/dialogaboutimpl.h \
    SPIERScommon/netmodule.h
SOURCES = src/mainwindowimpl.cpp \
    src/main.cpp \
    src/globals.cpp \
    src/scene.cpp \
    src/dialogaboutimpl.cpp \
    SPIERScommon/netmodule.cpp
RESOURCES = src/align.qrc
RC_FILE = src/exeicon.rc
TEMPLATE = app
