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

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    ../SPIERScommon/src/darkstyletheme.cpp \
    src/globals.cpp \
    src/meshfilters.cpp \
    src/positionclickhandler.cpp \
    src/scalegridoverlay.cpp \
    src/spvreader.cpp \
    src/svobject.cpp \
    src/spv.cpp \
    src/compressedslice.cpp \
    src/marchingcubes.cpp \
    src/dataconnectivityfilter.cpp \
    src/movetogroup.cpp \
    src/vaxml.cpp \
    src/quickhelpbox.cpp \
    src/aboutdialog.cpp \
    src/gl3widget.cpp \
    ../SPIERScommon/src/netmodule.cpp \
    ../SPIERScommon/src/updatedialog.cpp \
    ../SPIERScommon/src/semanticversion.cpp \
    ../SPIERScommon/src/prereleasecomponent.cpp \
    src/vaxmlgroup.cpp \
    src/vaxmlobject.cpp \
    src/isosurface.cpp \
    src/scalarfieldlayer.cpp \
    src/fullscreenwindow.cpp \
    src/spvwriter.cpp \
    src/staticfunctions.cpp \
    src/adler32.c \
    src/compress.c \
    src/crc32.c \
    src/deflate.c \
    src/infback.c \
    src/inffast.c \
    src/inflate.c \
    src/inftrees.c \
    src/trees.c \
    src/uncompr.c \
    src/zutil.c
HEADERS += src/mainwindow.h \
    ../SPIERScommon/src/darkstyletheme.h \
    src/globals.h \
    src/meshfilters.h \
    src/positionclickhandler.h \
    src/scalegridoverlay.h \
    src/spvreader.h \
    src/svobject.h \
    src/spv.h \
    src/compressedslice.h \
    src/marchingcubes.h \
    src/dataconnectivityfilter.h \
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
    src/vaxmlgroup.h \
    src/vaxmlobject.h \
    src/isosurface.h \
    src/scalarfieldlayer.h \
    src/fullscreenwindow.h \
    src/spvwriter.h \
    src/staticfunctions.h

FORMS += ui/mainwindow.ui \
    ui/movetogroup.ui \
    ui/quickhelpbox.ui \
    ui/aboutdialog.ui

# MacOS build here
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 26.0

    LIBS += -L/usr/local/homebrew/opt/icu4c/lib/ \
    -lstdc++

    INCLUDEPATH += /usr/local/homebrew/opt/icu4c/include/
    DEPENDPATH += /usr/local/homebrew/opt/icu4c/include/

    # Mac icon
    ICON = resources/SPIERSviewIcon.icns

    QMAKE_INFO_PLIST = Info.plist
}

# Unix/Linux common build here
unix:!macx {
    LIBS += -lstdc++
    #Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
    QMAKE_LFLAGS += -no-pie
}
