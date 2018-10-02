QT += network xml gui core opengl

TARGET = SPIERSview64

TEMPLATE = app

RESOURCES = view.qrc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += "C:\msys64\mingw64\include\vtk-8.1"

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
    ../SPIERScommon/netmodule.cpp \
    src/aboutdialog.cpp \
    src/gl3widget.cpp

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
    ../SPIERScommon/netmodule.h \
    src/aboutdialog.h \
    src/glvbo.h \
    src/gl3widget.h \
    src/rowmans.h \
    src/main.h \
    src/version.h

FORMS += ui/mainwindow.ui \
    ui/movetogroup.ui \
    ui/quickhelpbox.ui \
    ui/aboutdialog.ui

UI_DIR += ui

LIBS += -L"C:\msys64\mingw64\bin" \
# Removed to get it to compile with VTK 8.1
#-lvtkexpat-8.1 \
#-lvtkInfovisCore-8.1 \
#-lvtkalglib-8.1 \
#-lvtkfreetype-8.1 \
#-lvtkhdf5-8.1 \
#-lvtkhdf5_hl-8.1 \
#-lvtkjpeg-8.1 \
#-lvtkjsoncpp-8.1 \
#-lvtklibxml2-8.1 \
#-lvtkNetCDF_cxx-8.1 \
#-lvtkoggtheora-8.1 \
#-lvtkpng-8.1 \
#-lvtktiff-8.1 \
#-lvtkzlib-8.1 \
-lvtkCommonExecutionModel-8.1 \
-lvtkCommonDataModel-8.1 \
-lvtkCommonColor-8.1 \
-lvtkCommonComputationalGeometry-8.1 \
-lvtkCommonMisc-8.1 \
-lvtkCommonSystem-8.1 \
-lvtkCommonTransforms-8.1 \
-lvtkCommonMath-8.1 \
-lvtkDICOMParser-8.1 \
-lvtkDomainsChemistry-8.1 \
-lvtkDomainsChemistryOpenGL2-8.1 \
-lvtkexoIIc-8.1 \
-lvtkFiltersAMR-8.1 \
-lvtkFiltersExtraction-8.1 \
-lvtkFiltersFlowPaths-8.1 \
-lvtkFiltersGeneral-8.1 \
-lvtkFiltersGeneric-8.1 \
-lvtkFiltersGeometry-8.1 \
-lvtkFiltersHybrid-8.1 \
-lvtkFiltersHyperTree-8.1 \
-lvtkFiltersImaging-8.1 \
-lvtkFiltersModeling-8.1 \
-lvtkFiltersParallel-8.1 \
-lvtkFiltersParallelImaging-8.1 \
-lvtkFiltersProgrammable-8.1 \
-lvtkFiltersSelection-8.1 \
-lvtkFiltersSMP-8.1 \
-lvtkFiltersSources-8.1 \
-lvtkFiltersStatistics-8.1 \
-lvtkFiltersTexture-8.1 \
-lvtkFiltersVerdict-8.1 \
-lvtkglew-8.1 \
-lvtkImagingColor-8.1 \
-lvtkImagingFourier-8.1 \
-lvtkImagingGeneral-8.1 \
-lvtkImagingHybrid-8.1 \
-lvtkImagingMath-8.1 \
-lvtkImagingMorphological-8.1 \
-lvtkImagingSources-8.1 \
-lvtkImagingStatistics-8.1 \
-lvtkImagingStencil-8.1 \
-lvtkInfovisLayout-8.1 \
-lvtkIOAMR-8.1 \
-lvtkIOEnSight-8.1 \
-lvtkIOExodus-8.1 \
-lvtkIOGeometry-8.1 \
-lvtkIOImage-8.1 \
-lvtkIOImport-8.1 \
-lvtkIOInfovis-8.1 \
-lvtkIOLegacy-8.1 \
-lvtkIOLSDyna-8.1 \
-lvtkIOMINC-8.1 \
-lvtkIOMovie-8.1 \
-lvtkIONetCDF-8.1 \
-lvtkIOParallel-8.1 \
-lvtkIOParallelXML-8.1 \
-lvtkIOPLY-8.1 \
-lvtkIOSQL-8.1 \
-lvtkIOVideo-8.1 \
-lvtkIOXML-8.1 \
-lvtkIOXMLParser-8.1 \
-lvtkmetaio-8.1 \
-lvtkNetCDF-8.1 \
-lvtkRenderingContext2D-8.1 \
-lvtkRenderingContextOpenGL2-8.1 \
-lvtkRenderingFreeType-8.1 \
-lvtkRenderingOpenGL2-8.1 \
-lvtksqlite-8.1 \
-lvtkverdict-8.1 \
-lvtkCommonCore-8.1 \
-lvtkChartsCore-8.1 \
-lvtkImagingCore-8.1 \
-lvtkIOCore-8.1 \
-lvtkParallelCore-8.1 \
-lvtkRenderingCore-8.1 \
-lvtkFiltersCore-8.1 \
-lvtksys-8.1 \
    -lstdc++ \
    -lz \
    -lgdi32

DEPENDPATH += "C:\msys64\mingw64\include\vtk-8.1"

MOC_DIR += build

OBJECTS_DIR += build

RC_FILE = resources/icon.rc

DISTFILES += \
    LICENSE.md \
    .astylerc
