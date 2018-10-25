#------------------------------------------------------------------------------------------------
# SPIERSview
#------------------------------------------------------------------------------------------------

QT += network xml gui core opengl

# Allow debug in release
#QMAKE_CXXFLAGS_RELEASE += -g
#QMAKE_CFLAGS_RELEASE += -g
#QMAKE_LFLAGS_RELEASE =

TARGET = SPIERSview64

TEMPLATE = app

RESOURCES = view.qrc

DESTDIR \
    += \
    bin
	
# Load the SPIERS version number
include(../version.pri)

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
unix {
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

# MacOS common build here (not current tested or supported)
macx {
	LIBS += -L/Users/{user}/VTK-bin/lib/vtk-5.2 \
    -lvtkRendering \
    -lvtkGraphics \
    -lvtkImaging \
    -lvtkIO \
    -lvtkFiltering \
    -lvtkCommon \
    -lvtkftgl \
    -lvtkfreetype \
    -lvtkDICOMParser \
    -lvtkpng \
    -lvtktiff \
    -lvtkzlib \
    -lvtkjpeg \
    -lvtkexpat \
    -lvtksys \
    -lstdc++ \
    -mlibz.a
	INCLUDEPATH += "/Users/{user}/VTK-bin/include/vtk-5.2"
}

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/darkstyletheme.cpp \
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
    ../SPIERScommon/netmodule.cpp \
    ../SPIERScommon/semanticversion.cpp \
    ../SPIERScommon/prereleasecomponent.cpp \
    src/vaxmlgroup.cpp \
    src/vaxmlobject.cpp \
    src/isosurface.cpp \
    src/scalarfieldlayer.cpp \
    src/fullscreenwindow.cpp

HEADERS += src/mainwindow.h \
    src/darkstyletheme.h \
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
    src/glvbo.h \
    src/gl3widget.h \
    src/rowmans.h \
    src/main.h \
    ../SPIERScommon/netmodule.h \
    ../SPIERScommon/semanticversion.h \
    ../SPIERScommon/prereleasecomponent.h \
    src/vaxmlgroup.h \
    src/vaxmlobject.h \
    src/isosurface.h \
    src/scalarfieldlayer.h \
    src/fullscreenwindow.h

FORMS += ui/mainwindow.ui \
    ui/movetogroup.ui \
    ui/quickhelpbox.ui \
    ui/aboutdialog.ui

UI_DIR += ui

MOC_DIR += build

OBJECTS_DIR += build

RC_FILE = resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc
