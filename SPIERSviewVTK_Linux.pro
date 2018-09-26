QT += network xml gui core opengl
#Do not not to display warning messages for use of deprecated C++ functions by VTK.
QMAKE_CXXFLAGS += -Wno-deprecated

TARGET = SPIERSview2
TEMPLATE = app
RESOURCES = sview.qrc
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# RJG change for ubuntu - installed VTK from package, then just copied libraries folder into progamme directory: https://launchpad.net/~elvstone/+archive/ubuntu/vtk7
#Included libraries, and then RPATH in order that can load libraries on run, as long as they are in sub folder.
#INCLUDEPATH += "C:/Program Files (x86)/VTK/include/vtk-7.1"

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
MOC_DIR += build
OBJECTS_DIR += build
RC_FILE = icon.rc

#INCLUDEPATH += $$PWD/VTK-7.1.0/include
#INCLUDEPATH += $$PWD/VTK-7.0.0/include/vtk-7.0/
#DEPENDPATH += $$PWD/VTK-7.0.0/include
#DEPENDPATH += $$PWD/VTK-7.0.0/lib/

#INCLUDEPATH += $$PWD/vtk-7.1/
INCLUDEPATH +=/usr/include/vtk-7.1/
#DEPENDPATH += $$PWD/vtk-7.1/

#RJG - this ensures that as long as the VTK libraries are in the distribution folder, the software will launch
#QMAKE_RPATHDIR += $$PWD/VTK-7.0.0/lib/
#QMAKE_RPATHDIR += $$PWD/vtk-7.1/

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/VTK-7.0.0/lib/release/ -lvtkCommonExecutionModel-7.0
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/VTK-7.0.0/lib/debug/ -lvtkCommonExecutionModel-7.0
#else:unix: LIBS += -L$$PWD/VTK-7.0.0/lib/ -lvtkCommonExecutionModel-7.0 \

#LIBS += -L$PWD/vtk-7.1/ \
#-lvtkInfovisCore-7.0 \
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
#-lvtkglew-7.1 \
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
#-lvtksqlite-7.1 \
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
#Linux VTK 7.1 doesn't seem to have these - will become obvious if needed down line.
#Try fix with proper VTK install
#-lvtkexpat-7.1 \
#-lvtkfreetype-7.1 \
#-lvtkhdf5-7.1 \
#-lvtkhdf5_hl-7.1 \
#-lvtkjpeg-7.1 \
#-lvtkjsoncpp-7.1 \
#-lvtklibxml2-7.1 \
#-lvtkNetCDF-7.1 \
#-lvtkNetCDF_cxx-7.1 \
#-lvtkoggtheora-7.1 \
#-lvtkpng-7.1 \
#-lvtktiff-7.1 \
#-lvtkzlib-7.1 \
#-lgdi32 \

