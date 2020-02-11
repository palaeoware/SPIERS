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
    ../SPIERScommon/commonresources.qrc

UI_DIR = build

# Load the SPIERS version number
include(../version.pri)

DESTDIR \
    += \
    bin

#Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
QMAKE_LFLAGS += -no-pie

RC_FILE += resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR = build

OBJECTS_DIR = build

FORMS += ui/mainwindow.ui \
    ui/about.ui

#Mac icon
ICON = resources/SPIERSalign.icns

HEADERS += src/mainwindowimpl.h \
    src/globals.h \
    src/scene.h \
    ../SPIERScommon/src/darkstyletheme.h \
    src/about.h \
    ../SPIERScommon/src/netmodule.h \
    ../SPIERScommon/src/semanticversion.h \
    ../SPIERScommon/src/prereleasecomponent.h

SOURCES += src/mainwindowimpl.cpp \
    src/main.cpp \
    src/globals.cpp \
    src/scene.cpp \
    ../SPIERScommon/src/darkstyletheme.cpp \
    src/about.cpp \
    ../SPIERScommon/src/netmodule.cpp \
    ../SPIERScommon/src/semanticversion.cpp \
    ../SPIERScommon/src/prereleasecomponent.cpp

