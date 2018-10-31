#------------------------------------------------------------------------------------------------
# SPIERSalign
#------------------------------------------------------------------------------------------------

TARGET = SPIERSalign64

TEMPLATE += app

QT = gui core network widgets

CONFIG += qt \
    release \
    warn_on

RESOURCES += align.qrc \
    ../SPIERScommon/licenseresource.qrc

UI_DIR = build

# Load the SPIERS version number
include(../version.pri)

DESTDIR \
    += \
    bin

RC_FILE += resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR = build

OBJECTS_DIR = build

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

