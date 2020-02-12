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
    release \
    warn_on

RESOURCES = view.qrc \
    ../SPIERScommon/commonresources.qrc

DESTDIR \
    += \
    bin
	
UI_DIR += ui

# Load the SPIERS version number
include(../version.pri)

#Needed to make binaries launchable from file in Ubuntu - GCC default link flag -pie on newer Ubuntu versions this so otherwise recognised as shared library
QMAKE_LFLAGS += -no-pie

RC_FILE = resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc

MOC_DIR += build

OBJECTS_DIR += build

# Windows common build here
win32 {
    INCLUDEPATH += "$$PWD/../../../../../msys64/mingw64/include/vtk-8.1"

    LIBS += -L$$PWD/../../../../../msys64/mingw64/lib \
    # VTK - Libary
    # libvtk* name to match static lib created
    -llibvtkCommonExecutionModel-8.1 \
    -llibvtkCommonDataModel-8.1 \
    -llibvtkCommonColor-8.1 \
    -llibvtkCommonComputationalGeometry-8.1 \
    -llibvtkCommonMisc-8.1 \
    -llibvtkCommonSystem-8.1 \
    -llibvtkCommonTransforms-8.1 \
    -llibvtkCommonMath-8.1 \
    -llibvtkDICOMParser-8.1 \
    -llibvtkDomainsChemistry-8.1 \
    -llibvtkDomainsChemistryOpenGL2-8.1 \
    -llibvtkexoIIc-8.1 \
    -llibvtkFiltersAMR-8.1 \
    -llibvtkFiltersExtraction-8.1 \
    -llibvtkFiltersFlowPaths-8.1 \
    -llibvtkFiltersGeneral-8.1 \
    -llibvtkFiltersGeneric-8.1 \
    -llibvtkFiltersGeometry-8.1 \
    -llibvtkFiltersHybrid-8.1 \
    -llibvtkFiltersHyperTree-8.1 \
    -llibvtkFiltersImaging-8.1 \
    -llibvtkFiltersModeling-8.1 \
    -llibvtkFiltersParallel-8.1 \
    -llibvtkFiltersParallelImaging-8.1 \
    -llibvtkFiltersProgrammable-8.1 \
    -llibvtkFiltersSelection-8.1 \
    -llibvtkFiltersSMP-8.1 \
    -llibvtkFiltersSources-8.1 \
    -llibvtkFiltersStatistics-8.1 \
    -llibvtkFiltersTexture-8.1 \
    -llibvtkFiltersVerdict-8.1 \
    -llibvtkglew-8.1 \
    -llibvtkImagingColor-8.1 \
    -llibvtkImagingFourier-8.1 \
    -llibvtkImagingGeneral-8.1 \
    -llibvtkImagingHybrid-8.1 \
    -llibvtkImagingMath-8.1 \
    -llibvtkImagingMorphological-8.1 \
    -llibvtkImagingSources-8.1 \
    -llibvtkImagingStatistics-8.1 \
    -llibvtkImagingStencil-8.1 \
    -llibvtkInfovisLayout-8.1 \
    -llibvtkIOAMR-8.1 \
    -llibvtkIOEnSight-8.1 \
    -llibvtkIOExodus-8.1 \
    -llibvtkIOGeometry-8.1 \
    -llibvtkIOImage-8.1 \
    -llibvtkIOImport-8.1 \
    -llibvtkIOInfovis-8.1 \
    -llibvtkIOLegacy-8.1 \
    -llibvtkIOLSDyna-8.1 \
    -llibvtkIOMINC-8.1 \
    -llibvtkIOMovie-8.1 \
    -llibvtkIONetCDF-8.1 \
    -llibvtkIOParallel-8.1 \
    -llibvtkIOParallelXML-8.1 \
    -llibvtkIOPLY-8.1 \
    -llibvtkIOSQL-8.1 \
    -llibvtkIOVideo-8.1 \
    -llibvtkIOXML-8.1 \
    -llibvtkIOXMLParser-8.1 \
    -llibvtkmetaio-8.1 \
    -llibvtkNetCDF-8.1 \
    -llibvtkRenderingContext2D-8.1 \
    -llibvtkRenderingContextOpenGL2-8.1 \
    -llibvtkRenderingFreeType-8.1 \
    -llibvtkRenderingOpenGL2-8.1 \
    -llibvtksqlite-8.1 \
    -llibvtkverdict-8.1 \
    -llibvtkCommonCore-8.1 \
    -llibvtkChartsCore-8.1 \
    -llibvtkImagingCore-8.1 \
    -llibvtkIOCore-8.1 \
    -llibvtkParallelCore-8.1 \
    -llibvtkRenderingCore-8.1 \
    -llibvtkFiltersCore-8.1 \
    -llibvtksys-8.1 \
    # Other - Libary
    -lstdc++ \
    -lz \
    -lgdi32
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
		-lstdc++ \
		-lz \
}

# MacOS common build here (not currently tested or supported)
macx {

     #RJG - Below vtk installation achieved using installation via homebrew
     #eg. brew install vtk
     LIBS += -L$$PWD/../../../../../../usr/local/Cellar/vtk/8.2.0_7/lib/ \
    -lvtkCommonExecutionModel-8.2.1 \
    -lvtkCommonDataModel-8.2.1 \
    -lvtkCommonColor-8.2.1 \
    -lvtkCommonComputationalGeometry-8.2.1 \
    -lvtkCommonMisc-8.2.1 \
    -lvtkCommonSystem-8.2.1 \
    -lvtkCommonTransforms-8.2.1 \
    -lvtkCommonMath-8.2.1 \
    -lvtkDICOMParser-8.2.1 \
    -lvtkDomainsChemistry-8.2.1 \
    -lvtkDomainsChemistryOpenGL2-8.2.1 \
    -lvtkFiltersAMR-8.2.1 \
    -lvtkFiltersExtraction-8.2.1 \
    -lvtkFiltersFlowPaths-8.2.1 \
    -lvtkFiltersGeneral-8.2.1 \
    -lvtkFiltersGeneric-8.2.1 \
    -lvtkFiltersGeometry-8.2.1 \
    -lvtkFiltersHybrid-8.2.1 \
    -lvtkFiltersHyperTree-8.2.1 \
    -lvtkFiltersImaging-8.2.1 \
    -lvtkFiltersModeling-8.2.1 \
    -lvtkFiltersParallel-8.2.1 \
    -lvtkFiltersParallelImaging-8.2.1 \
    -lvtkFiltersProgrammable-8.2.1 \
    -lvtkFiltersSelection-8.2.1 \
    -lvtkFiltersSMP-8.2.1 \
    -lvtkFiltersSources-8.2.1 \
    -lvtkFiltersStatistics-8.2.1 \
    -lvtkFiltersTexture-8.2.1 \
    -lvtkFiltersVerdict-8.2.1 \
    -lvtkglew-8.2.1 \
    -lvtkImagingColor-8.2.1 \
    -lvtkImagingFourier-8.2.1 \
    -lvtkImagingGeneral-8.2.1 \
    -lvtkImagingHybrid-8.2.1 \
    -lvtkImagingMath-8.2.1 \
    -lvtkImagingMorphological-8.2.1 \
    -lvtkImagingSources-8.2.1 \
    -lvtkImagingStatistics-8.2.1 \
    -lvtkImagingStencil-8.2.1 \
    -lvtkInfovisLayout-8.2.1 \
    -lvtkIOAMR-8.2.1 \
    -lvtkIOEnSight-8.2.1 \
    -lvtkIOExodus-8.2.1 \
    -lvtkIOGeometry-8.2.1 \
    -lvtkIOImage-8.2.1 \
    -lvtkIOImport-8.2.1 \
    -lvtkIOInfovis-8.2.1 \
    -lvtkIOLegacy-8.2.1 \
    -lvtkIOLSDyna-8.2.1 \
    -lvtkIOMINC-8.2.1 \
    -lvtkIOMovie-8.2.1 \
    -lvtkIONetCDF-8.2.1 \
    -lvtkIOParallel-8.2.1 \
    -lvtkIOParallelXML-8.2.1 \
    -lvtkIOPLY-8.2.1 \
    -lvtkIOSQL-8.2.1 \
    -lvtkIOVideo-8.2.1 \
    -lvtkIOXML-8.2.1 \
    -lvtkIOXMLParser-8.2.1 \
    -lvtkmetaio-8.2.1 \
    -lvtkRenderingContext2D-8.2.1 \
    -lvtkRenderingContextOpenGL2-8.2.1 \
    -lvtkRenderingFreeType-8.2.1 \
    -lvtkRenderingOpenGL2-8.2.1 \
    -lvtksqlite-8.2.1 \
    -lvtkverdict-8.2.1 \
    -lvtkCommonCore-8.2.1 \
    -lvtkChartsCore-8.2.1 \
    -lvtkImagingCore-8.2.1 \
    -lvtkIOCore-8.2.1 \
    -lvtkParallelCore-8.2.1 \
    -lvtkRenderingCore-8.2.1 \
    -lvtkFiltersCore-8.2.1 \
    -lvtksys-8.2.1 \
    -lstdc++ \
    -lz

     INCLUDEPATH += $$PWD/../../../../../../usr/local/Cellar/vtk/8.2.0_7/include/vtk-8.2/
     DEPENDPATH += $$PWD/../../../../../../usr/local/Cellar/vtk/8.2.0_7/include/vtk-8.2/

    #RJG - Note also the virtual machine has a VTK version built on the box:
    #LIBS += -L$$PWD/../../../../SPIERS/VTK-8.2.0/build/lib/ \
    #INCLUDEPATH += $$PWD/../../../../SPIERS/VTK-8.2.0/build
    #DEPENDPATH += $$PWD/../../../../SPIERS/VTK-8.2.0/build
    #I have left this in case an alternative is ever required


    #Mac icon
    ICON = resources/SPIERSviewIcon.icns

}

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
