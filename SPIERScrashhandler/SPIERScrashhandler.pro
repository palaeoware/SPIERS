TARGET = SPIERScrashhandler64
TEMPLATE = app

QT += core gui network widgets

CONFIG += qt debug_and_release warn_on sdk_no_version_check

RESOURCES = ../SPIERScommon/commonresources.qrc

UI_DIR += ui
DESTDIR += bin
MOC_DIR += build
OBJECTS_DIR += build

include(../version.pri)

HEADERS += src/crashhandler.h \
        src/crashdialog.h

SOURCES += src/main.cpp \
        src/crashhandler.cpp \
        src/crashdialog.cpp

win32 {
    RC_FILE = resources/icon.rc
}

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 26.0
    ICON = resources/SPIERScrashhandlerIcon.icns
    QMAKE_INFO_PLIST = Info.plist
}

unix:!macx {
    QMAKE_LFLAGS += -no-pie
}
