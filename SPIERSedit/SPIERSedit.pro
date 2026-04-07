#------------------------------------------------------------------------------------------------
# SPIERSedit
#------------------------------------------------------------------------------------------------

TARGET = SPIERSedit64

TEMPLATE = app

QT += core \
    gui \
    network \
    widgets \
    opengl \
    openglwidgets \
    concurrent

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

DESTDIR \
    += \
    bin

RC_FILE = resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR += build

OBJECTS_DIR += build

FORMS += ui/newprojectdialog.ui \
    ui/mainwindow.ui \
    ui/copying.ui \
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
    src/newprojectdialog.h \
    src/labelledpoint.h \
    src/mainwindow.h \
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
    src/mlfeaturepresets.h \
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
    src/mlparallelforest.h \
    src/mlupdateblockingdialog.h \
    src/myscene.h \
    src/copyingimpl.h \
    src/exportspv.h \
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
    src/previewwidget.h \
    ../SPIERScommon/src/customstyletheme.h \
    ../SPIERScommon/src/advancedpreferencesdialog.h \
    ../SPIERScommon/src/netmodule.h \
    ../SPIERScommon/src/updatedialog.h \
    ../SPIERScommon/src/semanticversion.h \
    ../SPIERScommon/src/prereleasecomponent.h \
    ui/mladdfeature.h

macx: HEADERS += src/main.h

SOURCES += src/display.cpp \
    src/beamhardening.cpp \
    src/beamhardeningcentericon.cpp \
    src/brush.cpp \
    src/bytearray2d.cpp \
    src/curves.cpp \
    src/fileio.cpp \
    src/globals.cpp \
    src/newprojectdialog.cpp \
    src/labelledpoint.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
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
    src/mlfeaturepresets.cpp \
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
    src/mlparallelforest.cpp \
    src/mlupdateblockingdialog.cpp \
    src/myscene.cpp \
    src/copyingimpl.cpp \
    src/exportspv.cpp \
    src/moreimpl.cpp \
    src/undo.cpp \
    src/contrastimpl.cpp \
    src/mainwindow2.cpp \
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
    src/keysafespinbox.cpp \
    src/mainview.cpp \
    src/previewwidget.cpp \
    ../SPIERScommon/src/customstyletheme.cpp \
    ../SPIERScommon/src/advancedpreferencesdialog.cpp \
    ../SPIERScommon/src/netmodule.cpp \
    ../SPIERScommon/src/updatedialog.cpp \
    ../SPIERScommon/src/semanticversion.cpp \
    ../SPIERScommon/src/prereleasecomponent.cpp \
    ui/mladdfeature.cpp

win32 {
    # Auto-detect OpenCV installation directory if not already set externally
    isEmpty(OPENCV_DIR) {
        USER_PROFILE = $$(USERPROFILE)
        OPENCV_CANDIDATE_DIRS = \
            C:/opencv/opencv \
            C:/opencv \
            C:/tools/opencv \
            C:/local/opencv \
            $$USER_PROFILE/Downloads/opencv/opencv \
            $$USER_PROFILE/Downloads/opencv \
            $$USER_PROFILE/Documents/opencv/opencv \
            $$USER_PROFILE/Documents/opencv
        for(dir, OPENCV_CANDIDATE_DIRS) {
            exists($$dir/build/include) {
                isEmpty(OPENCV_DIR): OPENCV_DIR = $$dir
            }
        }
    }
    isEmpty(OPENCV_DIR): error("OpenCV not found. Install it or set OPENCV_DIR (e.g. qmake OPENCV_DIR=C:/my/opencv).")

    INCLUDEPATH += $$OPENCV_DIR/build/include
    LIBS += -L$$OPENCV_DIR/build/x64/vc16/lib

    # Auto-detect OpenCV version by globbing for the release lib (4-digit version, no trailing 'd')
    OPENCV_RELEASE_LIBS = $$files($$OPENCV_DIR/build/x64/vc16/lib/opencv_world????.lib)
    isEmpty(OPENCV_RELEASE_LIBS): error("No opencv_world lib found in $$OPENCV_DIR/build/x64/vc16/lib/")
    OPENCV_LIB_FILE = $$first(OPENCV_RELEASE_LIBS)
    OPENCV_LIB_NAME = $$basename(OPENCV_LIB_FILE)
    OPENCV_LIB_NAME = $$replace(OPENCV_LIB_NAME, \.lib, )

    CONFIG(debug, debug|release) {
        LIBS += -l$${OPENCV_LIB_NAME}d
        OPENCV_DLL_NAME = $${OPENCV_LIB_NAME}d.dll
    } else {
        LIBS += -l$$OPENCV_LIB_NAME
        OPENCV_DLL_NAME = $${OPENCV_LIB_NAME}.dll
    }

    # Copy OpenCV DLL to the build output directory so the app runs from Qt Creator
    OPENCV_DLL_SRC = $$shell_quote($$shell_path($$OPENCV_DIR/build/x64/vc16/bin/$$OPENCV_DLL_NAME))
    OPENCV_DLL_DST = $$shell_quote($$shell_path($$OUT_PWD/bin/))
    QMAKE_POST_LINK += xcopy /y /i /q $$OPENCV_DLL_SRC $$OPENCV_DLL_DST
}

# MacOS common build here
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 26.0

    OPENCV_DIR = /opt/homebrew/opt/opencv

    INCLUDEPATH += $$OPENCV_DIR/include/opencv4
    LIBS += -L$$OPENCV_DIR/lib \
            -lopencv_core \
            -lopencv_imgproc \
            -lopencv_imgcodecs \
            -lopencv_highgui \
            -lopencv_ml

    ICON = resources/SPIERSeditIcon.icns
    QMAKE_INFO_PLIST = Info.plist
}

# Unix/Linux common build here
unix:!macx {
    #Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
    QMAKE_LFLAGS += -no-pie

    # Open CV installed using: sudo apt install libopencv-dev
    # Location shown by:  dpkg -L libopencv-dev
    OPENCV_DIR = /usr/lib/x86_64-linux-gnu/
    INCLUDEPATH += /usr/include/opencv4/

    LIBS += -L$$OPENCV_DIR/lib \
    -lopencv_core \
    -lopencv_imgproc \
    -lopencv_imgcodecs \
    -lopencv_highgui \
    -lopencv_ml
}

