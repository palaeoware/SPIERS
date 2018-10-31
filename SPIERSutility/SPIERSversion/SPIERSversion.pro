#------------------------------------------------------------------------------------------------
# SPIERSversion
#------------------------------------------------------------------------------------------------

TARGET = SPIERSversion

TEMPLATE = app

QT += network xml gui core widgets

CONFIG += qt \
    release \
    warn_on

RESOURCES = version.qrc \
    ../../SPIERScommon/licenseresource.qrc

DESTDIR \
    += \
    bin
	
UI_DIR += ui

# Load the SPIERS version number
include(../../version.pri)

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR += build

OBJECTS_DIR += build

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/darkstyletheme.cpp \
    src/globals.cpp \
    src/aboutdialog.cpp \

HEADERS += src/mainwindow.h \
    src/darkstyletheme.h \
    src/globals.h \
    src/main.h \
    src/aboutdialog.h \

FORMS += ui/mainwindow.ui \
    ui/aboutdialog.ui
