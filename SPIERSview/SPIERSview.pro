#------------------------------------------------------------------------------------------------
# SPIERSview
#------------------------------------------------------------------------------------------------

TARGET = SPIERSview64

TEMPLATE = app

QT += network xml gui core widgets opengl

# Allow debug in release
#QMAKE_CXXFLAGS_RELEASE += -g
#QMAKE_CFLAGS_RELEASE += -g
#QMAKE_LFLAGS_RELEASE =

CONFIG += qt \
    #release \
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

unix:!macx {
    #Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
    QMAKE_LFLAGS += -no-pie
}

RC_FILE = resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR += build

OBJECTS_DIR += build

# Windows VS2019 build here
win32 {
    INCLUDEPATH += "C://Users/as13709/Documents/Programming/VTK/9.3.0/Install/include/vtk-9.3" \
    "C://Users/as13709/Documents/Programming/ZLIB/1.3/Install/include"

    release {
        LIBS += -LC://Users/as13709/Documents/Programming/VTK/9.3.0/Install/lib \
        # VTK - Libary
        # vtk* name to match lib created
        -lvtkCommonCore-9.3 \
        -lvtkCommonDataModel-9.3 \
        -lvtkCommonExecutionModel-9.3 \
        -lvtkCommonMath-9.3 \
        -lvtkCommonMisc-9.3 \
        -lvtkCommonSystem-9.3 \
        -lvtkCommonTransforms-9.3 \
        -lvtkdoubleconversion-9.3 \
        -lvtkFiltersCore-9.3 \
        -lvtkFiltersGeneral-9.3 \
        -lvtkFiltersHybrid-9.3 \
        -lvtkFiltersVerdict-9.3 \
        -lvtkIOCore-9.3 \
        -lvtkIOGeometry-9.3 \
        -lvtkIOImage-9.3 \
        -lvtkIOPLY-9.3 \
        -lvtkkissfft-9.3 \
        -lvtkloguru-9.3 \
        -lvtklz4-9.3 \
        -lvtklzma-9.3 \
        -lvtkpugixml-9.3 \
        -lvtkRenderingCore-9.3 \
        -lvtksys-9.3 \
        -lvtkzlib-9.3 \
        # Zlib
        -LC://Users/as13709/Documents/Programming/ZLIB/1.3/Install/lib \
        -lzlib
    }

    debug {
        LIBS += -LC://Users/as13709/Documents/Programming/VTK/9.3.0/Install/lib \
        # VTK - Libary
        # vtk* name to match lib created
        -lvtkCommonCore-9.3d \
        -lvtkCommonDataModel-9.3d \
        -lvtkCommonExecutionModel-9.3d \
        -lvtkCommonMath-9.3d \
        -lvtkCommonMisc-9.3d \
        -lvtkCommonSystem-9.3d \
        -lvtkCommonTransforms-9.3d \
        -lvtkdoubleconversion-9.3d \
        -lvtkFiltersCore-9.3d \
        -lvtkFiltersGeneral-9.3d \
        -lvtkFiltersHybrid-9.3d \
        -lvtkFiltersVerdict-9.3d \
        -lvtkIOCore-9.3d \
        -lvtkIOGeometry-9.3d \
        -lvtkIOImage-9.3d \
        -lvtkIOPLY-9.3d \
        -lvtkkissfft-9.3d \
        -lvtkloguru-9.3d \
        -lvtklz4-9.3d \
        -lvtklzma-9.3d \
        -lvtkpugixml-9.3d \
        -lvtkRenderingCore-9.3d \
        -lvtksys-9.3d \
        -lvtkzlib-9.3d \
        # Zlib
        -LC://Users/as13709/Documents/Programming/ZLIB/1.3/Install/lib \
        -lzlibd
    }
}

# Unix/Linux common build here
unix:!macx {
    #Libraries from VTK package (e.g. sudo apt-get install libvtk7-qt-dev).
    INCLUDEPATH +=/usr/include/vtk-7.1/
    LIBS += -L/usr/include/vtk-7.1/ \
    -lvtkCommonExecutionModel-7.1 \
    -lvtkCommonDataModel-7.1 \
    -lvtkCommonColor-7.1 \
    -lvtkCommonComputationalGeometry-7.1 \
    -lvtkCommonMisc-7.1 \
    -lvtkCommonSystem-7.1 \
    -lvtkCommonTransforms-7.1 \
    -lvtkCommonMath-7.1 \
    -lvtkalglib-7.1 \
    -lvtkDICOMParser-7.1 \
    -lvtkDomainsChemistry-7.1 \
    -lvtkDomainsChemistryOpenGL2-7.1 \
    -lvtkexoIIc-7.1 \
    -lvtkFiltersAMR-7.1 \
    -lvtkFiltersExtraction-7.1 \
    -lvtkFiltersFlowPaths-7.1 \
    -lvtkFiltersGeneral-7.1 \
    -lvtkFiltersGeneric-7.1 \
    -lvtkFiltersGeometry-7.1 \
    -lvtkFiltersHybrid-7.1 \
    -lvtkFiltersHyperTree-7.1 \
    -lvtkFiltersImaging-7.1 \
    -lvtkFiltersModeling-7.1 \
    -lvtkFiltersParallel-7.1 \
    -lvtkFiltersParallelImaging-7.1 \
    -lvtkFiltersProgrammable-7.1 \
    -lvtkFiltersSelection-7.1 \
    -lvtkFiltersSMP-7.1 \
    -lvtkFiltersSources-7.1 \
    -lvtkFiltersStatistics-7.1 \
    -lvtkFiltersTexture-7.1 \
    -lvtkFiltersVerdict-7.1 \
    -lvtkImagingColor-7.1 \
    -lvtkImagingFourier-7.1 \
    -lvtkImagingGeneral-7.1 \
    -lvtkImagingHybrid-7.1 \
    -lvtkImagingMath-7.1 \
    -lvtkImagingMorphological-7.1 \
    -lvtkImagingSources-7.1 \
    -lvtkImagingStatistics-7.1 \
    -lvtkImagingStencil-7.1 \
    -lvtkInfovisLayout-7.1 \
    -lvtkIOAMR-7.1 \
    -lvtkIOEnSight-7.1 \
    -lvtkIOExodus-7.1 \
    -lvtkIOGeometry-7.1 \
    -lvtkIOImage-7.1 \
    -lvtkIOImport-7.1 \
    -lvtkIOInfovis-7.1 \
    -lvtkIOLegacy-7.1 \
    -lvtkIOLSDyna-7.1 \
    -lvtkIOMINC-7.1 \
    -lvtkIOMovie-7.1 \
    -lvtkIONetCDF-7.1 \
    -lvtkIOParallel-7.1 \
    -lvtkIOParallelXML-7.1 \
    -lvtkIOPLY-7.1 \
    -lvtkIOSQL-7.1 \
    -lvtkIOVideo-7.1 \
    -lvtkIOXML-7.1 \
    -lvtkIOXMLParser-7.1 \
    -lvtkmetaio-7.1 \
    -lvtkRenderingContext2D-7.1 \
    -lvtkRenderingContextOpenGL2-7.1 \
    -lvtkRenderingFreeType-7.1 \
    -lvtkRenderingOpenGL2-7.1 \
    -lvtkverdict-7.1 \
    -lvtkCommonCore-7.1 \
    -lvtkChartsCore-7.1 \
    -lvtkImagingCore-7.1 \
    -lvtkIOCore-7.1 \
    -lvtkParallelCore-7.1 \
    -lvtkRenderingCore-7.1 \
    -lvtkFiltersCore-7.1 \
    -lvtksys-7.1 \
    # Other - Libary
    -lstdc++ \
    -lz \
}

# MacOS x86_64 common build here
macx {
    # Below vtk installation achieved using installation via homebrew:
    #     brew install vtk
    # On MacOS running on arm64 (M1 chip) homebrew must be installed under x86_64 to work with QT 5.x
    # To do this:
    #     cd ~/Downloads
    #     mkdir homebrew
    #     curl -L https://github.com/Homebrew/brew/tarball/master | tar xz --strip 1 -C homebrew
    #     sudo mv homebrew /usr/local/homebrew
    #     export PATH=$HOME/bin:/usr/local/bin:$PATH
    #     alias brew_86='arch -x86_64 /usr/local/homebrew/bin/brew'
    #     brew_86 insall vtk
    # If this is not done then homebrew with get installed under arm64 architecture making the VTK
    # libs incompatible with the x86_64 QT complie that is installed by default from the QT binary.
    # QT 6.2 claims to be a universal build (i.e. x86_64 + arm64) but VTK cannot currently be build
    # as a universal library. As such on arm64 MacOS machines we are targeting the Intel x86_64
    # so that it runs on older Macs and under Rosetta on arm64 (M1 chip) machines. Given more time
    # both QT and VTK might both catch up with the newer Apple silicon around.
    LIBS += -L/usr/local/homebrew/opt/icu4c/lib/ \
    -licudata \
    -licuuc \
    -L/usr/local/homebrew/Cellar/vtk/9.2.6/lib/ \
    -lvtkCommonExecutionModel-9.2.1 \
    -lvtkCommonDataModel-9.2.1 \
    -lvtkCommonColor-9.2.1 \
    -lvtkCommonComputationalGeometry-9.2.1 \
    -lvtkCommonMisc-9.2.1 \
    -lvtkCommonSystem-9.2.1 \
    -lvtkCommonTransforms-9.2.1 \
    -lvtkCommonMath-9.2.1 \
    -lvtkDICOMParser-9.2.1 \
    -lvtkDomainsChemistry-9.2.1 \
    -lvtkDomainsChemistryOpenGL2-9.2.1 \
    -lvtkFiltersAMR-9.2.1 \
    -lvtkFiltersExtraction-9.2.1 \
    -lvtkFiltersFlowPaths-9.2.1 \
    -lvtkFiltersGeneral-9.2.1 \
    -lvtkFiltersGeneric-9.2.1 \
    -lvtkFiltersGeometry-9.2.1 \
    -lvtkFiltersHybrid-9.2.1 \
    -lvtkFiltersHyperTree-9.2.1 \
    -lvtkFiltersImaging-9.2.1 \
    -lvtkFiltersModeling-9.2.1 \
    -lvtkFiltersParallel-9.2.1 \
    -lvtkFiltersParallelImaging-9.2.1 \
    -lvtkFiltersProgrammable-9.2.1 \
    -lvtkFiltersSelection-9.2.1 \
    -lvtkFiltersSMP-9.2.1 \
    -lvtkFiltersSources-9.2.1 \
    -lvtkFiltersStatistics-9.2.1 \
    -lvtkFiltersTexture-9.2.1 \
    -lvtkFiltersVerdict-9.2.1 \
    -lvtkImagingColor-9.2.1 \
    -lvtkImagingFourier-9.2.1 \
    -lvtkImagingGeneral-9.2.1 \
    -lvtkImagingHybrid-9.2.1 \
    -lvtkImagingMath-9.2.1 \
    -lvtkImagingMorphological-9.2.1 \
    -lvtkImagingSources-9.2.1 \
    -lvtkImagingStatistics-9.2.1 \
    -lvtkImagingStencil-9.2.1 \
    -lvtkInfovisLayout-9.2.1 \
    -lvtkIOAMR-9.2.1 \
    -lvtkIOEnSight-9.2.1 \
    -lvtkIOExodus-9.2.1 \
    -lvtkIOGeometry-9.2.1 \
    -lvtkIOImage-9.2.1 \
    -lvtkIOImport-9.2.1 \
    -lvtkIOInfovis-9.2.1 \
    -lvtkIOLegacy-9.2.1 \
    -lvtkIOLSDyna-9.2.1 \
    -lvtkIOMINC-9.2.1 \
    -lvtkIOMovie-9.2.1 \
    -lvtkIONetCDF-9.2.1 \
    -lvtkIOParallel-9.2.1 \
    -lvtkIOParallelXML-9.2.1 \
    -lvtkIOPLY-9.2.1 \
    -lvtkIOSQL-9.2.1 \
    -lvtkIOVideo-9.2.1 \
    -lvtkIOXML-9.2.1 \
    -lvtkIOXMLParser-9.2.1 \
    -lvtkmetaio-9.2.1 \
    -lvtkRenderingContext2D-9.2.1 \
    -lvtkRenderingContextOpenGL2-9.2.1 \
    -lvtkRenderingFreeType-9.2.1 \
    -lvtkRenderingOpenGL2-9.2.1 \
    -lvtkverdict-9.2.1 \
    -lvtkCommonCore-9.2.1 \
    -lvtkChartsCore-9.2.1 \
    -lvtkImagingCore-9.2.1 \
    -lvtkIOCore-9.2.1 \
    -lvtkParallelCore-9.2.1 \
    -lvtkRenderingCore-9.2.1 \
    -lvtkFiltersCore-9.2.1 \
    -lvtksys-9.2.1 \
    # Other - Libary
    -lstdc++ \
    -lz

    INCLUDEPATH += /usr/local/homebrew/Cellar/vtk/9.2.6/include/vtk-9.2/
    DEPENDPATH += /usr/local/homebrew/Cellar/vtk/9.2.6/include/vtk-9.2/
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

# RJG - Note also the virtual machine has a VTK version built on the box:
#macx {
    #LIBS += -L$$PWD/../../../../SPIERS/VTK-9.1.0/build/lib/ \
    #INCLUDEPATH += $$PWD/../../../../SPIERS/VTK-9.1.0/build
    #DEPENDPATH += $$PWD/../../../../SPIERS/VTK-9.1.0/build
    # I have left this in case an alternative is ever required

    # Mac icon
    #ICON = resources/SPIERSviewIcon.icns

    # macOS file associations are done through the Info.plist files under the application pakages
    # QT/qmake should be able to overwrite the default generated file with this custom file
    # We need to make sure that the custom Info.plist (below) is in XML format and not binary
    # as qmake uses sed for string replacements within it.
    #QMAKE_INFO_PLIST = Info.plist
#}

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    ../SPIERScommon/src/darkstyletheme.cpp \
    src/globals.cpp \
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
    ../SPIERScommon/src/semanticversion.cpp \
    ../SPIERScommon/src/prereleasecomponent.cpp \
    src/vaxmlgroup.cpp \
    src/vaxmlobject.cpp \
    src/isosurface.cpp \
    src/scalarfieldlayer.cpp \
    src/fullscreenwindow.cpp \
    src/spvwriter.cpp \
    src/staticfunctions.cpp \
    src/drawglscalegrid.cpp \
    src/drawglscaleball.cpp \
    src/gridfontsizedialog.cpp

HEADERS += src/mainwindow.h \
    ../SPIERScommon/src/darkstyletheme.h \
    src/globals.h \
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
    src/rowmans.h \
    src/main.h \
    ../SPIERScommon/src/netmodule.h \
    ../SPIERScommon/src/semanticversion.h \
    ../SPIERScommon/src/prereleasecomponent.h \
    src/vaxmlgroup.h \
    src/vaxmlobject.h \
    src/isosurface.h \
    src/scalarfieldlayer.h \
    src/fullscreenwindow.h \
    src/spvwriter.h \
    src/staticfunctions.h \
    src/drawglscalegrid.h \
    src/drawglscaleball.h \
    src/gridfontsizedialog.h

FORMS += ui/mainwindow.ui \
    ui/movetogroup.ui \
    ui/quickhelpbox.ui \
    ui/aboutdialog.ui \
    ui/gridfontsizedialog.ui
