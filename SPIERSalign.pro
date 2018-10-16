#------------------------------------------------------------------------------------------------
# SPIERSalign
#------------------------------------------------------------------------------------------------

TARGET = SPIERSalign64

TEMPLATE += app

QT = gui core network widgets

CONFIG += qt \
    release \
    warn_on

DESTDIR = bin

OBJECTS_DIR = build

MOC_DIR = build

UI_DIR = build

FORMS += ui/mainwindow.ui \
    ui/about.ui

HEADERS += src/mainwindowimpl.h \
    src/globals.h \
    src/scene.h \
    src/darkstyletheme.h \
    src/about.h \
    ../SPIERScommon/netmodule.h \
    ../SPIERScommon/semanticversion.h \
    ../SPIERScommon/prereleasecomponent.h

SOURCES += src/mainwindowimpl.cpp \
    src/main.cpp \
    src/globals.cpp \
    src/scene.cpp \
    src/darkstyletheme.cpp \
    src/about.cpp \
   ../SPIERScommon/netmodule.cpp \
    ../SPIERScommon/semanticversion.cpp \
    ../SPIERScommon/prereleasecomponent.cpp

RESOURCES += align.qrc

RC_FILE += resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc
