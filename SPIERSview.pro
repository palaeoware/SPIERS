#------------------------------------------------------------------------------------------------
# SPIERSview
#------------------------------------------------------------------------------------------------

QT += network xml gui core opengl

# Allow debug in release
QMAKE_CXXFLAGS_RELEASE += -g
QMAKE_CFLAGS_RELEASE += -g
QMAKE_LFLAGS_RELEASE =

TARGET = SPIERSview64

TEMPLATE = app

RESOURCES = view.qrc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += "$$PWD/../../../../msys64/mingw64/include/vtk-8.1"

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/darkstyletheme.cpp \
    src/SPIERSviewglobals.cpp \
    src/spvreader.cpp \
    src/svobject.cpp \
    src/spv.cpp \
    src/compressedslice.cpp \
    src/mc.cpp \
    src/myconnectivityfilter.cpp \
    src/movetogroup.cpp \
    src/voxml.cpp \
    src/quickhelpbox.cpp \
    src/aboutdialog.cpp \
    src/gl3widget.cpp \
    ../SPIERScommon/netmodule.cpp \
    ../SPIERScommon/semanticversion.cpp \
    ../SPIERScommon/prereleasecomponent.cpp


HEADERS += src/mainwindow.h \
    src/darkstyletheme.h \
    src/SPIERSviewglobals.h \
    src/spvreader.h \
    src/svobject.h \
    src/spv.h \
    src/compressedslice.h \
    src/mc.h \
    src/myconnectivityfilter.h \
    src/movetogroup.h \
    src/voxml.h \
    src/quickhelpbox.h \
    src/aboutdialog.h \
    src/glvbo.h \
    src/gl3widget.h \
    src/rowmans.h \
    src/main.h \
    src/version.h \
    ../SPIERScommon/netmodule.h \
    ../SPIERScommon/semanticversion.h \
    ../SPIERScommon/prereleasecomponent.h

LIBS += -L$$PWD/../../../../msys64/mingw64/lib \
# VTK - Libary
# libvtk* name to match static lib created
# Removed to get it to compile with VTK 8.1
#-llibvtkexpat-8.1 \
#-llibvtkInfovisCore-8.1 \
#-llibvtkalglib-8.1 \
#-llibvtkfreetype-8.1 \
#-llibvtkhdf5-8.1 \
#-llibvtkhdf5_hl-8.1 \
#-llibvtkjpeg-8.1 \
#-llibvtkjsoncpp-8.1 \
#-llibvtklibxml2-8.1 \
#-llibvtkNetCDF_cxx-8.1 \
#-llibvtkoggtheora-8.1 \
#-llibvtkpng-8.1 \
#-llibvtktiff-8.1 \
#-llibvtkzlib-8.1 \
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
