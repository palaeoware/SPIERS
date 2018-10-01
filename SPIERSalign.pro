#------------------------------------------------------------------------------------------------
# SPIERS Align
#------------------------------------------------------------------------------------------------

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
    ../SPIERScommon/netmodule.h \
    src/darkstyletheme.h \
    src/version.h \
    src/about.h

SOURCES += src/mainwindowimpl.cpp \
    src/main.cpp \
    src/globals.cpp \
    src/scene.cpp \
    ../SPIERScommon/netmodule.cpp \
    src/darkstyletheme.cpp \
    src/about.cpp

RESOURCES += align.qrc

RC_FILE += resources/icon.rc

TEMPLATE += app

DISTFILES += \
    LICENSE.md \
    .astylerc
