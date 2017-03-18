QT += network xml gui core opengl
TARGET = SPIERSview2
TEMPLATE = app
RESOURCES = sview.qrc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# RJG change for ubuntu - installed from here: https://launchpad.net/~elvstone/+archive/ubuntu/vtk7
#INCLUDEPATH += "C:/Program Files (x86)/VTK/include/vtk-7.0"
INCLUDEPATH += ./VTK-7.0.0
INCLUDEPATH += ./VTK-7.0.0/include/vtk-7.0

SOURCES += main.cpp \
    mainwindow.cpp \
    SPIERSviewglobals.cpp \
    spvreader.cpp \
    svobject.cpp \
    spv.cpp \
    compressedslice.cpp \
    mc.cpp \
    myconnectivityfilter.cpp \
    movetogroup.cpp \
    voxml.cpp \
    quickhelpbox.cpp \
    ../SPIERScommon/netmodule.cpp \
    aboutdialog.cpp \
    gl3widget.cpp

HEADERS += mainwindow.h \
    SPIERSviewglobals.h \
    spvreader.h \
    svobject.h \
    spv.h \
    compressedslice.h \
    mc.h \
    myconnectivityfilter.h \
    movetogroup.h \
    voxml.h \
    quickhelpbox.h \
    ../SPIERScommon/netmodule.h \
    aboutdialog.h \
    glvbo.h \
    gl3widget.h \
    rowmans.h \
    main.h

FORMS += mainwindow.ui \
    movetogroup.ui \
    quickhelpbox.ui \
    aboutdialog.ui

#RJG for linux
DEPENDPATH += ./VTK-7.0.0/lib/
MOC_DIR += build
OBJECTS_DIR += build
RC_FILE = icon.rc


#RJG change for ubuntu
LIBS += -L/opt/VTK-7.0.0/lib \ #-L"C:/program files (x86)/vtk/lib" \
 -L/opt/VTK-7.0.0/include/vtk-7.0 \
#-lvtkInfovisCore-7.0 \
-lvtkCommonExecutionModel-7.0 \
-lvtkCommonDataModel-7.0 \
-lvtkCommonColor-7.0 \
-lvtkCommonComputationalGeometry-7.0 \
-lvtkCommonMisc-7.0 \
-lvtkCommonSystem-7.0 \
-lvtkCommonTransforms-7.0 \
-lvtkCommonMath-7.0 \
#-lvtkalglib-7.0 \
-lvtkDICOMParser-7.0 \
-lvtkDomainsChemistry-7.0 \
-lvtkDomainsChemistryOpenGL2-7.0 \
-lvtkexoIIc-7.0 \
-lvtkFiltersAMR-7.0 \
-lvtkFiltersExtraction-7.0 \
-lvtkFiltersFlowPaths-7.0 \
-lvtkFiltersGeneral-7.0 \
-lvtkFiltersGeneric-7.0 \
-lvtkFiltersGeometry-7.0 \
-lvtkFiltersHybrid-7.0 \
-lvtkFiltersHyperTree-7.0 \
-lvtkFiltersImaging-7.0 \
-lvtkFiltersModeling-7.0 \
-lvtkFiltersParallel-7.0 \
-lvtkFiltersParallelImaging-7.0 \
-lvtkFiltersProgrammable-7.0 \
-lvtkFiltersSelection-7.0 \
-lvtkFiltersSMP-7.0 \
-lvtkFiltersSources-7.0 \
-lvtkFiltersStatistics-7.0 \
-lvtkFiltersTexture-7.0 \
-lvtkFiltersVerdict-7.0 \
-lvtkglew-7.0 \
-lvtkImagingColor-7.0 \
-lvtkImagingFourier-7.0 \
-lvtkImagingGeneral-7.0 \
-lvtkImagingHybrid-7.0 \
-lvtkImagingMath-7.0 \
-lvtkImagingMorphological-7.0 \
-lvtkImagingSources-7.0 \
-lvtkImagingStatistics-7.0 \
-lvtkImagingStencil-7.0 \
-lvtkInfovisLayout-7.0 \
-lvtkIOAMR-7.0 \
-lvtkIOEnSight-7.0 \
-lvtkIOExodus-7.0 \
-lvtkIOGeometry-7.0 \
-lvtkIOImage-7.0 \
-lvtkIOImport-7.0 \
-lvtkIOInfovis-7.0 \
-lvtkIOLegacy-7.0 \
-lvtkIOLSDyna-7.0 \
-lvtkIOMINC-7.0 \
-lvtkIOMovie-7.0 \
-lvtkIONetCDF-7.0 \
-lvtkIOParallel-7.0 \
-lvtkIOParallelXML-7.0 \
-lvtkIOPLY-7.0 \
-lvtkIOSQL-7.0 \
-lvtkIOVideo-7.0 \
-lvtkIOXML-7.0 \
-lvtkIOXMLParser-7.0 \
-lvtkmetaio-7.0 \
-lvtkRenderingContext2D-7.0 \
-lvtkRenderingContextOpenGL2-7.0 \
-lvtkRenderingFreeType-7.0 \
-lvtkRenderingOpenGL2-7.0 \
-lvtksqlite-7.0 \
-lvtkverdict-7.0 \
-lvtkCommonCore-7.0 \
-lvtkChartsCore-7.0 \
-lvtkImagingCore-7.0 \
-lvtkIOCore-7.0 \
-lvtkParallelCore-7.0 \
-lvtkRenderingCore-7.0 \
-lvtkFiltersCore-7.0 \
-lvtksys-7.0 \
    -lstdc++ \
    -lz \
#Linux VTK 7.0 doesn't seem to have these - will become obvious if needed down line.
#Try fix with proper VTK install
#-lvtkexpat-7.0 \
#-lvtkfreetype-7.0 \
#-lvtkhdf5-7.0 \
#-lvtkhdf5_hl-7.0 \
#-lvtkjpeg-7.0 \
#-lvtkjsoncpp-7.0 \
#-lvtklibxml2-7.0 \
#-lvtkNetCDF-7.0 \
#-lvtkNetCDF_cxx-7.0 \
#-lvtkoggtheora-7.0 \
#-lvtkpng-7.0 \
#-lvtktiff-7.0 \
#-lvtkzlib-7.0 \
#-lgdi32 \

