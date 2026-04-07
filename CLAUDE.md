# SPIERS — Claude Code Context

## What is SPIERS?

**Serial Palaeontological Image Editing and Rendering System** — a suite of four Qt6 C++ desktop applications for reconstructing and analysing tomographic (CT/serial-section) fossil datasets.

| App | Target binary | Purpose |
|-----|--------------|---------|
| **SPIERSedit** | `SPIERSedit64` | Image-stack editor; paints segment masks, runs ML feature extraction, exports SPV/DXF |
| **SPIERSview** | `SPIERSview64` | 3D viewer for SPV and VAXML+STL formats; OpenGL rendering, marching cubes |
| **SPIERSalign** | `SPIERSalign64` | Manual image-stack alignment tool |
| **SPIERSversion** | `SPIERSversion` | Lightweight version/update utility |

- **Licence:** GNU GPL v3
- **Authors:** Mark D. Sutton, Russell J. Garwood, Alan R.T. Spencer
- **Repo:** https://github.com/palaeoware/SPIERS
- **Active branch:** `Qt6-and-AI`
- **Current version:** 4.0.0 (defined in `version.pri`)

---

## Repository Layout

```
SPIERS/
├── SPIERS.pro               # Root qmake subdirs project
├── version.pri              # Defines SOFTWARE_VERSION for all apps
├── SPIERSedit/              # Edit application
├── SPIERSview/              # View application
├── SPIERSalign/             # Align application
├── SPIERSutility/
│   └── SPIERSversion/       # Version/update utility
├── SPIERScommon/            # Shared library (theme, net, versioning)
│   ├── src/                 # Shared .h/.cpp files
│   ├── resources/           # Shared assets
│   │   └── customstyle/     # QSS stylesheets + icon PNGs
│   └── commonresources.qrc
├── resources/               # Root-level logos and icons
├── docs/                    # Sphinx documentation (per-app + root)
├── inno/                    # Windows installer scripts
└── examples/                # Linux .desktop entry examples
```

Each application follows the same internal layout:

```
SPIERSxxxx/
├── SPIERSxxxx.pro
├── src/          # .h and .cpp files
├── ui/           # Qt Designer .ui forms + generated headers
├── resources/    # App-specific icons, .rc file
├── bin/          # Output directory for compiled binary (DESTDIR)
└── build/        # MOC/object intermediates (MOC_DIR / OBJECTS_DIR)
```

---

## Build System

- **qmake** — do not switch to CMake.
- Open `SPIERS.pro` in Qt Creator (Qt6, Fusion style kit) for a full build.
- Individual apps can be opened via their own `.pro`.
- The `version.pri` file is `include()`d by every app `.pro`; change the version string there only.
- All four apps include `../SPIERScommon/commonresources.qrc` in `RESOURCES`.
- Platform guards: `win32 {}`, `macx {}`, `unix:!macx {}`.
- **Never** add a source file to only one `.pro` if it belongs to `SPIERScommon` — add it to all four `.pro` files under `HEADERS`/`SOURCES` using the `../SPIERScommon/src/` relative path.

---

## SPIERScommon — Shared Code

All cross-app functionality lives here. Key files:

| File | Role |
|------|------|
| `customstyletheme.h/.cpp` | `CustomStyleTheme : QProxyStyle` — dark/light theme, palette, QSS loading |
| `advancedpreferencesdialog.h/.cpp` | Theme-switcher dialog (used by view, align, version) |
| `netmodule.h/.cpp` | Update-check network module |
| `updatedialog.h/.cpp` | Download / install-update dialog |
| `semanticversion.h/.cpp` | SemVer parser |
| `prereleasecomponent.h/.cpp` | Pre-release tag comparator |

SPIERSedit has its own settings dialog (`SPIERSedit/ui/settings.ui` + `settingsimpl.cpp`) that embeds the theme combo directly rather than using `AdvancedPreferencesDialog`.

---

## Theme System

The custom theme is driven by `CustomStyleTheme` (a `QProxyStyle` over Fusion) plus two QSS stylesheets.

### ThemeMode enum
```cpp
enum class ThemeMode { Dark, Light, System };
```

### Key entry points
| Call | When |
|------|------|
| `CustomStyleTheme::readThemeSetting()` | `main()` — read saved preference |
| `QApplication::setStyle(new CustomStyleTheme(mode))` | `main()` — install at startup |
| `CustomStyleTheme::applyToApplication(mode)` | Live switch from preferences dialog |
| `CustomStyleTheme::writeThemeSetting(mode)` | Called inside `applyToApplication` |

### Settings key
`QSettings("Palaeoware", "SPIERS")` → key `"Theme"` (int cast of `ThemeMode`). Shared across all four apps.

### Resource paths
- `:/customstyle/stylesheet_dark.qss`
- `:/customstyle/stylesheet_light.qss`
- Icons: `:/customstyle/icon_*.png`
- QRC prefix: `/customstyle` (in `SPIERScommon/commonresources.qrc`)

### QSS rules — critical constraints
- **No `background-color` on global `QWidget`** — triggers `WA_StyledBackground`, breaks dock sizing.
- **No QSS rules on `QSpinBox` / `QDoubleSpinBox` / `QDateEdit`** — any CSS property kills Fusion's native arrow drawing.
- **No `QComboBox::drop-down` subcontrol** — hides the dropdown arrow.
- **No `QToolButton` global rules** — scope to `QToolBar QToolButton` only.
- `QMainWindow::separator` needs `padding: 2px` to be grabbable.

---

## Coding Conventions

### File & class naming
- Source files: `lowercase_with_underscores.h` / `.cpp`
- Classes: `PascalCase`
- Qt slots: `on_<widgetName>_<signalName>()` pattern
- Member variables: `m_camelCase` for new code; existing code uses bare `camelCase`

### Every source file must start with:
```cpp
/**
 * @file
 * Header: <Brief description>
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2019 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; ...
 */
```

### Header guards
```cpp
#ifndef MYFILENAME_H
#define MYFILENAME_H
// ...
#endif // MYFILENAME_H
```

### Qt idioms
- Prefer `QString`, `QList`, `QVector`, `QMap` over STL equivalents.
- Use `Q_OBJECT` in every class that uses signals/slots.
- Connect with new-style syntax: `connect(src, &Src::sig, dst, &Dst::slot)`.
- Use `QStringLiteral("...")` for compile-time string literals.
- Const-correctness: mark methods `const` where possible.

### Code formatting
- Each app has an `.astylerc` — run AStyle before committing.
- Indentation: 4 spaces (no tabs).

---

## OpenGL Notes (SPIERSedit / SPIERSview)

- Uses Qt's `QOpenGLWidget` / `QOpenGLFunctions` wrappers.
- `SPIERSview` embeds zlib source directly (`adler32.c`, `compress.c`, etc.) — do not replace with system zlib.
- OpenCV is optional (SPIERSedit ML features only); guarded by `#ifdef` in the `.pro`.

---

## Documentation

- Each app has a `docs/` subdirectory with reStructuredText (`.rst`) files.
- Built via Sphinx multiproject; config in root `conf.py`.
- Hosted on Read the Docs: https://spiers-software.org/
- **Do not auto-generate Markdown documentation** — the project uses `.rst` only.

---

## What NOT to Do

- Do not switch from qmake to CMake.
- Do not convert `.pro` targets or binary names (keep `SPIERSedit64`, `SPIERSview64`, etc.).
- Do not add VTK dependencies — removed in the Qt6 branch.
- Do not replace `QProxyStyle`/Fusion theming with a third-party style library.
- Do not use `QApplication::setStyleSheet()` (static call fails in Qt6) — use `qApp->setStyleSheet()`.
- Do not create new `.md` documentation files unless explicitly asked.
- Do not add emoji to source code or comments.
