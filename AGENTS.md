# SPIERS — Codex Context

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

Full specification: https://github.com/palaeoware/repoconventions
Detailed rules for Codex sessions: `.Codex/rules/cpp-qt-conventions.md`

### Naming

| Thing | Convention |
|-------|-----------|
| Classes | PascalCase (uppercase first letter) |
| Functions / methods | camelCase (lowercase first letter) |
| Variables | camelCase (lowercase first letter) |
| Constructors / destructors | Match class name (exception to camelCase rule) |
| Member variables | camelCase; `m_camelCase` in new code |
| Constants | `UPPER_SNAKE` |
| Source files | `lowercase.h` / `.cpp` (no underscores) |
| Qt auto-slots | `on_<widget>_<signal>()` |

- One variable per line. No short or meaningless names.
- Single-character names only for loop counters and obvious temporaries.

### Pointers and References

```cpp
int *pointer;      // space before *, not after
QString &ref;      // space before &, not after
```

### Braces — Non-Attached Style

Opening brace on a **new line** for all constructs:
```cpp
void MyClass::myFunction()
{
    if (condition)
    {
        doSomething();
    }
}
```

### Defines / Includes / Declarations Order

Within every translation unit:
1. `#define` macros (below file header, blank line after)
2. Project `.h` includes, then Qt includes — one per line, each group alphabetical, blank line between groups
3. Variable declarations — `static`/`extern`/`constexpr` before plain, alphabetical by type

### Line Length

Soft maximum **200 characters**. Break long signatures onto multiple lines when it aids readability; skip the break if it makes the code harder to follow.

### Comments (Doxygen)

```cpp
/**
 *
 * Block comment for functions, classes, and notable code sections.
 *
 **/

int segmentCount; /// Inline variable comment
```

### File Header

Every `.h` and `.cpp` opens with the GPL boilerplate (see `.Codex/rules/cpp-qt-conventions.md` for the exact text).

### Header Guards

```cpp
#ifndef MYFILENAME_H
#define MYFILENAME_H
// ...
#endif // MYFILENAME_H
```

### Qt idioms
- Prefer `QString`, `QList`, `QVector`, `QMap` over STL equivalents.
- `Q_OBJECT` in every class using signals/slots.
- New-style connect: `connect(src, &Src::sig, dst, &Dst::slot)`.
- `QStringLiteral("...")` for compile-time string literals.
- Const-correct: mark methods `const` where they don't mutate state.

### Formatting
- 4-space indentation, no tabs.
- Run AStyle with the app's `.astylerc` before committing.

---

## Versioning

Releases follow **Semantic Versioning** (semver.org) and **PEP 440**:
- Version defined once in `version.pri`: `DEFINES += SOFTWARE_VERSION='\\"4.0.0\\"'`
- Release branches named `origin/vN.N.N` (e.g. `origin/v4.0.0`) — Read the Docs parses these automatically.
- Do not hard-code the version string anywhere other than `version.pri`.

---

## SPE File Format Compatibility Policy (SPIERSedit)

When changing SPIERSedit `.spe` serialization, preserve cross-version safety with this rule:

1. **Append-only layout:** add new serialized fields at end-of-file only.
2. **Never reorder existing fields:** keep all historic read/write ordering intact.
3. **Old-reader safety:** old builds should stop reading before new appended data and still parse their known prefix correctly.
4. **Version bumps are last resort:** bump `SPEFILEVERSION` only when append-only cannot express the change safely.

Practical guidance for implementation:

- Keep `WriteSettings()` existing field order unchanged; append new sections after current tail.
- Keep `ReadSettings()` backwards-friendly by guarding extra trailing reads with `!in.atEnd()`.
- For incompatible future files (`version > SPEFILEVERSION`), fail closed (do not attempt partial/best-effort load).

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
