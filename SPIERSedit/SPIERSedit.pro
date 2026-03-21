#------------------------------------------------------------------------------------------------
# SPIERSedit
#------------------------------------------------------------------------------------------------

TARGET = SPIERSedit64

TEMPLATE = app

QT += core \
    gui \
    network \
    widgets

CONFIG += qt \
    debug_and_release \
    warn_on \
    thread \
    sdk_no_version_check

RESOURCES = SPIERSedit.qrc \
    ../SPIERScommon/commonresources.qrc

UI_DIR += ui

# Load the SPIERS version number
include(../version.pri)

#Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
QMAKE_LFLAGS += -no-pie

DESTDIR \
    += \
    bin

RC_FILE = resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR += build

OBJECTS_DIR += build

FORMS += ui/import.ui \
    ui/mainwindow.ui \
    ui/Copying.ui \
    ui/mladdfeature.ui \
    ui/more.ui \
    ui/outputsettings.ui \
    ui/contrast.ui \
    ui/deletemask.ui \
    ui/settings.ui \
    ui/selectsegment.ui \
    ui/curveresize.ui \
    ui/slicespacing.ui \
    ui/distribute.ui \
    ui/resample.ui \
    ui/about.ui

HEADERS += src/display.h \
    src/beamhardening.h \
    src/beamhardeningcentericon.h \
    src/brush.h \
    src/bytearray2d.h \
    src/curves.h \
    src/fileio.h \
    src/globals.h \
    src/importdialogimpl.h \
    src/labelledpoint.h \
    src/mainwindowimpl.h \
    src/mlcachedaccess.h \
    src/mlcachedslice.h \
    src/mlfeature.h \
    src/mlfeaturecontrast.h \
    src/mlfeaturedifferenceofgaussians.h \
    src/mlfeaturegaussian.h \
    src/mlfeaturegradient.h \
    src/mlfeaturegradientcomponent.h \
    src/mlfeaturehessian.h \
    src/mlfeatureintensity.h \
    src/mlfeaturelog.h \
    src/mlfeaturesquareintensity.h \
    src/mlfeaturetensorcoherencelocal.h \
    src/mlfeaturetensorcoherencewide.h \
    src/mlfeaturetensorcomponentlocal.h \
    src/mlfeaturetensorcomponentwide.h \
    src/mlfeaturetensordeterminantlocal.h \
    src/mlfeaturetensordeterminantwide.h \
    src/mlfeaturetensortracelocal.h \
    src/mlfeaturetensortracewide.h \
    src/mlfeatureuimanager.h \
    src/mlfeaturevariance.h \
    src/mlfileio.h \
    src/mlinterface.h \
    src/mlupdateblockingdialog.h \
    src/myscene.h \
    src/copyingimpl.h \
    src/moreimpl.h \
    src/undo.h \
    src/contrastimpl.h \
    src/deletemaskdialogimpl.h \
    src/settingsimpl.h \
    src/histogram.h \
    src/selectsegmentimpl.h \
    src/resizedialogimpl.h \
    src/output.h \
    src/exportdxf.h \
    src/info.h \
    src/slicespacingdialogimpl.h \
    src/myrangescene.h \
    src/mygraphicsview.h \
    src/distributedialogimpl.h \
    src/resampleimpl.h \
    src/dialogaboutimpl.h \
    src/keysafespinbox.h \
    src/mainview.h \
    src/main.h \
    ../SPIERScommon/src/darkstyletheme.h \
    ../SPIERScommon/src/netmodule.h \
    ../SPIERScommon/src/semanticversion.h \
    ../SPIERScommon/src/prereleasecomponent.h \
    ui/mladdfeature.h

SOURCES += src/display.cpp \
    src/beamhardening.cpp \
    src/beamhardeningcentericon.cpp \
    src/brush.cpp \
    src/bytearray2d.cpp \
    src/curves.cpp \
    src/fileio.cpp \
    src/globals.cpp \
    src/importdialogimpl.cpp \
    src/labelledpoint.cpp \
    src/main.cpp \
    src/mainwindowimpl.cpp \
    src/mlcachedaccess.cpp \
    src/mlcachedslice.cpp \
    src/mlfeature.cpp \
    src/mlfeaturecontrast.cpp \
    src/mlfeaturedifferenceofgaussians.cpp \
    src/mlfeaturegaussian.cpp \
    src/mlfeaturegradient.cpp \
    src/mlfeaturegradientcomponent.cpp \
    src/mlfeaturehessian.cpp \
    src/mlfeatureintensity.cpp \
    src/mlfeaturelog.cpp \
    src/mlfeaturesquareintensity.cpp \
    src/mlfeaturetensorcoherencelocal.cpp \
    src/mlfeaturetensorcoherencewide.cpp \
    src/mlfeaturetensorcomponentlocal.cpp \
    src/mlfeaturetensorcomponentwide.cpp \
    src/mlfeaturetensordeterminantlocal.cpp \
    src/mlfeaturetensordeterminantwide.cpp \
    src/mlfeaturetensortracelocal.cpp \
    src/mlfeaturetensortracewide.cpp \
    src/mlfeatureuimanager.cpp \
    src/mlfeaturevariance.cpp \
    src/mlfileio.cpp \
    src/mlinterface.cpp \
    src/mlupdateblockingdialog.cpp \
    src/myscene.cpp \
    src/copyingimpl.cpp \
    src/moreimpl.cpp \
    src/undo.cpp \
    src/contrastimpl.cpp \
    src/mainwindowimpl2.cpp \
    src/deletemaskdialogimpl.cpp \
    src/settingsimpl.cpp \
    src/histogram.cpp \
    src/selectsegmentimpl.cpp \
    src/resizedialogimpl.cpp \
    src/output.cpp \
    src/exportdxf.cpp \
    src/info.cpp \
    src/slicespacingdialogimpl.cpp \
    src/myrangescene.cpp \
    src/mygraphicsvew.cpp \
    src/distributedialogimpl.cpp \
    src/resampleimpl.cpp \
    src/dialogaboutimpl.cpp \
    src/mainwindow3.cpp \
    src/copyingimpl2_ouput.cpp \
    src/keysafespinbox.cpp \
    src/mainview.cpp \
    ../SPIERScommon/src/darkstyletheme.cpp \
    ../SPIERScommon/src/netmodule.cpp \
    ../SPIERScommon/src/semanticversion.cpp \
    ../SPIERScommon/src//prereleasecomponent.cpp \
    ui/mladdfeature.cpp

OPENCV_DIR = C:/opencv/opencv
INCLUDEPATH += $$OPENCV_DIR/build/include
LIBS += -L$$OPENCV_DIR/build/x64/vc16/lib
CONFIG(debug, debug|release) {
        LIBS += -lopencv_world4130d
    } else {
        LIBS += -lopencv_world4130
    }
# MacOS common build here
macx {
    #Mac icon
    ICON = resources/SPIERSeditIcon.icns


    # macOS file associations are done through the Info.plist files under the application pakages
    # QT/qmake should be able to overwrite the default generated file with this custom file
    # We need to make sure that the custom Info.plist (below) is in XML format and not binary
    # as qmake uses sed for string replacements within it.
    QMAKE_INFO_PLIST = Info.plist
}
