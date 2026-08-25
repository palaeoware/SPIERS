# qmake Build System Rules

## Golden Rules

- **Do not switch to CMake.** All build files are `.pro` / `.pri`.
- **Do not rename binary targets** (`SPIERSedit64`, `SPIERSview64`, `SPIERSalign64`, `SPIERSversion`).
- **Do not change `DESTDIR` or `OBJECTS_DIR`** — existing scripts and Qt Creator expect `bin/` and `build/`.

## Adding a Source File

If a file is **app-specific**, add it only to that app's `.pro`:
```qmake
HEADERS += src/myfeature.h
SOURCES += src/myfeature.cpp
```

If a file is **shared** (belongs in `SPIERScommon/src/`), add it to **all four** `.pro` files:
```qmake
HEADERS += ../SPIERScommon/src/mysharedfile.h
SOURCES += ../SPIERScommon/src/mysharedfile.cpp
```

## Bumping the Version

Edit `version.pri` only:
```qmake
DEFINES += SOFTWARE_VERSION='\\"4.1.0\\"'
```
All four apps include this file via `include(../version.pri)`.

## Adding a Qt Module

Add to the `QT +=` block in the relevant `.pro`. Common modules:
- `core gui widgets network` — all apps
- `opengl openglwidgets` — SPIERSedit, SPIERSview
- `concurrent` — SPIERSedit, SPIERSview
- `xml` — SPIERSview, SPIERSversion

## Adding a Resource

1. Place the file in the appropriate `resources/` folder.
2. Add a `<file>` entry to the relevant `.qrc` with an `alias`.
3. For shared assets → `SPIERScommon/commonresources.qrc` (prefix `/customstyle` for theme assets).
4. For app-specific assets → `SPIERSxxxx/SPIERSxxxx.qrc`.

## Platform-Specific Blocks

```qmake
win32 {
    # Windows-only config
}

macx {
    # macOS-only config
}

unix:!macx {
    # Linux-only config
}
```

## Standard Per-App `.pro` Structure

```qmake
TARGET   = SPIERSxxxx64
TEMPLATE = app

QT      += core gui widgets network ...
CONFIG  += qt debug_and_release warn_on thread sdk_no_version_check

RESOURCES = SPIERSxxxx.qrc \
            ../SPIERScommon/commonresources.qrc

UI_DIR      += ui
DESTDIR     += bin
MOC_DIR     += build
OBJECTS_DIR += build

include(../version.pri)

HEADERS += src/mainwindow.h \
           ../SPIERScommon/src/customstyletheme.h \
           ...

SOURCES += src/main.cpp \
           src/mainwindow.cpp \
           ../SPIERScommon/src/customstyletheme.cpp \
           ...
```

## Running qmake After .pro / .qrc Changes

In Qt Creator: **Build → Run qmake**, then rebuild.
From the command line:
```
qmake SPIERS.pro && make -j4
```
Always run qmake (not just make) after changing `.pro`, `.pri`, or `.qrc` files.
