#------------------------------------------------------------------------------------------------
# SPIERSview
#------------------------------------------------------------------------------------------------
TARGET = SPIERSview64

TEMPLATE = app

QT += network xml gui core widgets opengl openglwidgets concurrent
# Allow debug in release
#QMAKE_CXXFLAGS_RELEASE += -g
#QMAKE_CFLAGS_RELEASE += -g
#QMAKE_LFLAGS_RELEASE =

CONFIG += qt \
    warn_on \
    sdk_no_version_check

RESOURCES = view.qrc \
    ../SPIERScommon/commonresources.qrc

DESTDIR \
    += \
    bin
	
UI_DIR += ui

# Load the SPIERS version number
include(../version.pri)

RC_FILE = resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR += build

OBJECTS_DIR += build

# Include path for zlib headers
INCLUDEPATH += src/zlib

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    ../SPIERScommon/src/customstyletheme.cpp \
    ../SPIERScommon/src/colourswatchlabel.cpp \
    ../SPIERScommon/src/themeselectorwidget.cpp \
    src/globals.cpp \
    src/meshfilters.cpp \
    src/positionclickhandler.cpp \
    src/scalegridoverlay.cpp \
    src/spvreader.cpp \
    src/svobject.cpp \
    src/spv.cpp \
    src/blenderbridge.cpp \
    src/compressedslice.cpp \
    src/fbxexporter.cpp \
    src/marchingcubes.cpp \
    src/dataconnectivityfilter.cpp \
    src/objexporter.cpp \
    src/movetogroup.cpp \
    src/vaxml.cpp \
    src/quickhelpbox.cpp \
    src/aboutdialog.cpp \
    src/gl3widget.cpp \
    ../SPIERScommon/src/netmodule.cpp \
    ../SPIERScommon/src/updatedialog.cpp \
    ../SPIERScommon/src/semanticversion.cpp \
    ../SPIERScommon/src/prereleasecomponent.cpp \
    ../SPIERScommon/src/crashdetector.cpp \
    src/vaxmlgroup.cpp \
    src/vaxmlobject.cpp \
    src/isosurface.cpp \
    src/scalarfieldlayer.cpp \
    src/fullscreenwindow.cpp \
    src/spvwriter.cpp \
    src/staticfunctions.cpp \
    src/settingsimpl.cpp \
    src/zlib/adler32.c \
    src/zlib/compress.c \
    src/zlib/crc32.c \
    src/zlib/deflate.c \
    src/zlib/infback.c \
    src/zlib/inffast.c \
    src/zlib/inflate.c \
    src/zlib/inftrees.c \
    src/zlib/trees.c \
    src/zlib/uncompr.c \
    src/zlib/zutil.c
HEADERS += src/mainwindow.h \
    ../SPIERScommon/src/customstyletheme.h \
    ../SPIERScommon/src/colourswatchlabel.h \
    ../SPIERScommon/src/themeselectorwidget.h \
    src/globals.h \
    src/meshfilters.h \
    src/positionclickhandler.h \
    src/scalegridoverlay.h \
    src/spvreader.h \
    src/svobject.h \
    src/spv.h \
    src/blenderbridge.h \
    src/compressedslice.h \
    src/fbxexporter.h \
    src/marchingcubes.h \
    src/dataconnectivityfilter.h \
    src/objexporter.h \
    src/movetogroup.h \
    src/vaxml.h \
    src/quickhelpbox.h \
    src/aboutdialog.h \
    src/gl3widget.h \
    src/main.h \
    ../SPIERScommon/src/netmodule.h \
    ../SPIERScommon/src/updatedialog.h \
    ../SPIERScommon/src/semanticversion.h \
    ../SPIERScommon/src/prereleasecomponent.h \
    ../SPIERScommon/src/crashdetector.h \
    src/vaxmlgroup.h \
    src/vaxmlobject.h \
    src/isosurface.h \
    src/scalarfieldlayer.h \
    src/fullscreenwindow.h \
    src/spvwriter.h \
    src/staticfunctions.h \
    src/settingsimpl.h \
    src/zlib/crc32.h \
    src/zlib/deflate.h \
    src/zlib/gzguts.h \
    src/zlib/inffast.h \
    src/zlib/inffixed.h \
    src/zlib/inflate.h \
    src/zlib/inftrees.h \
    src/zlib/trees.h \
    src/zlib/zconf.h \
    src/zlib/zlib.h \
    src/zlib/zutil.h

FORMS += ui/mainwindow.ui \
    ui/movetogroup.ui \
    ui/quickhelpbox.ui \
    ui/aboutdialog.ui \
    ui/settings.ui

# MacOS build here
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 13.0
    QMAKE_APPLE_DEVICE_ARCHS = arm64 x86_64

    LIBS += -L/usr/local/homebrew/opt/icu4c/lib/ \
    -lstdc++

    INCLUDEPATH += /usr/local/homebrew/opt/icu4c/include/
    DEPENDPATH += /usr/local/homebrew/opt/icu4c/include/

    # Mac icon
    ICON = resources/SPIERSviewIcon.icns

    # macOS file associations are done through the Info.plist files under the application pakages
    # QT/qmake should be able to overwrite the default generated file with this custom file
    # We need to make sure that the custom Info.plist (below) is in XML format and not binary
    # as qmake uses sed for string replacements within it.
    QMAKE_INFO_PLIST = Info.plist
}

# Unix/Linux common build here
unix:!macx {
    LIBS += -lstdc++
    #Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
    QMAKE_LFLAGS += -no-pie
}
