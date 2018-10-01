#------------------------------------------------------------------------------------------------
# SPIERS Edit
#------------------------------------------------------------------------------------------------

CONFIG += warn_on \
    qt \
    thread \

DESTDIR \
    += \
    bin

FORMS += ui/import.ui \
    ui/mainwindow.ui \
    ui/Copying.ui \
    ui/more.ui \
    ui/outputsettings.ui \
    ui/contrast.ui \
    ui/deletemask.ui \
    ui/settings.ui \
    ui/selectsegment.ui \
    ui/curveresize.ui \
    ui/slicespacing.ui \
    ui/findpolynomial.ui \
    ui/distribute.ui \
    ui/resample.ui \
    ui/about.ui

HEADERS += src/display.h \
    src/brush.h \
    src/curves.h \
    src/fileio.h \
    src/globals.h \
    src/importdialogimpl.h \
    src/mainwindowimpl.h \
    src/myscene.h \
    src/copyingimpl.h \
    src/moreimpl.h \
    src/undo.h \
    src/contrastimpl.h \
    src/deletemaskdialogimpl.h \
    src/settingsimpl.h \
    src/backthread.h \
    src/histogram.h \
    src/selectsegmentimpl.h \
    src/resizedialogimpl.h \
    src/output.h \
    src/exportdxf.h \
    src/info.h \
    src/slicespacingdialogimpl.h \
    src/findpolynomialimpl.h \
    src/myrangescene.h \
    src/mygraphicsview.h \
    src/distributedialogimpl.h \
    src/resampleimpl.h \
    src/dialogaboutimpl.h \
    ../SPIERScommon/netmodule.h \
    src/keysafespinbox.h \
    src/mainview.h \
    src/darkstyletheme.h \
    src/version.h

MOC_DIR += build

OBJECTS_DIR += build

QT += core \
    gui \
    network \
    widgets

SOURCES += src/display.cpp \
    src/brush.cpp \
    src/curves.cpp \
    src/fileio.cpp \
    src/globals.cpp \
    src/importdialogimpl.cpp \
    src/main.cpp \
    src/mainwindowimpl.cpp \
    src/myscene.cpp \
    src/copyingimpl.cpp \
    src/moreimpl.cpp \
    src/undo.cpp \
    src/contrastimpl.cpp \
    src/mainwindowimpl2.cpp \
    src/deletemaskdialogimpl.cpp \
    src/settingsimpl.cpp \
    src/backthread.cpp \
    src/histogram.cpp \
    src/selectsegmentimpl.cpp \
    src/resizedialogimpl.cpp \
    src/output.cpp \
    src/exportdxf.cpp \
    src/info.cpp \
    src/slicespacingdialogimpl.cpp \
    src/findpolynomialimpl.cpp \
    src/myrangescene.cpp \
    src/mygraphicsvew.cpp \
    src/distributedialogimpl.cpp \
    src/resampleimpl.cpp \
    src/dialogaboutimpl.cpp \
    src/mainwindow3.cpp \
    ../SPIERScommon/netmodule.cpp \
    src/copyingimpl2_ouput.cpp \
    src/keysafespinbox.cpp \
    src/mainview.cpp \
    src/darkstyletheme.cpp

UI_DIR += ui

TEMPLATE = app

RESOURCES = SPIERSedit.qrc

ICON = resources/edit.icns

DISTFILES += \
    LICENSE.md \
    .astylerc
#LIBS += -L"C:\\Qt\\Qt5.1\\ported64\\lib" \
#    -llibz
