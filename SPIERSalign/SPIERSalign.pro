#------------------------------------------------------------------------------------------------
# SPIERSalign
#------------------------------------------------------------------------------------------------

TARGET = SPIERSalign64

TEMPLATE += app

QT = gui core network widgets

CONFIG += qt \
    release \
    warn_on \
    sdk_no_version_check

RESOURCES += align.qrc \
    ../SPIERScommon/commonresources.qrc

UI_DIR = build

# Load the SPIERS version number
include(../version.pri)

DESTDIR \
    += \
    bin

# Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
QMAKE_LFLAGS += -no-pie

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

# MacOS common build here
macx {
    #Mac icon
    ICON = resources/SPIERSalignIcon.icns


    # macOS file associations are done through the Info.plist files under the application pakages
    # QT/qmake should be able to overwrite the default generated file with this custom file
    # We need to make sure that the custom Info.plist (below) is in XML format and not binary
    # as qmake uses sed for string replacements within it.
    QMAKE_INFO_PLIST = Info.plist
}
