# Theme System Rules

## Architecture

The theme system is `CustomStyleTheme : public QProxyStyle` (Fusion base).

```
SPIERScommon/src/customstyletheme.h    — class + ThemeMode enum
SPIERScommon/src/customstyletheme.cpp  — palette + QSS loading
SPIERScommon/resources/customstyle/    — .qss files + icon PNGs
SPIERScommon/commonresources.qrc       — registers resources under :/customstyle/
```

## ThemeMode

```cpp
enum class ThemeMode { Dark, Light, System };
```
- `System` resolves via `QStyleHints::colorScheme()` (Qt 6.5+); falls back to `Dark`.
- Default when no QSettings key exists: **Dark**.

## Applying at Startup (main.cpp)

```cpp
QApplication::setStyle(new CustomStyleTheme(CustomStyleTheme::readThemeSetting()));
```
The `polish(QApplication*)` override loads the QSS and scales the font on HiDPI screens.

## Live Theme Switching

```cpp
CustomStyleTheme::applyToApplication(ThemeMode::Light);
```
This single call: saves the setting, rebuilds the palette, installs a new style object, and reloads the QSS — no restart needed.

## Persisting the Preference

```cpp
// Write:
CustomStyleTheme::writeThemeSetting(ThemeMode::Dark);

// Read (returns ThemeMode::Dark if key is absent):
ThemeMode mode = CustomStyleTheme::readThemeSetting();
```
Storage: `QSettings("Palaeoware", "SPIERS")` key `"Theme"` (int).

## Icon Inversion for Light Mode

White-on-transparent PNG icons need inverting in light mode. Pattern used at load sites:
```cpp
auto themedPixmap = [](const QString &path) -> QPixmap {
    QImage img(path);
    if (CustomStyleTheme::currentApplicationMode() != ThemeMode::Light)
        img.invertPixels(QImage::InvertRgb);
    return QPixmap::fromImage(img);
};
```

## QSS Hard Rules — DO NOT VIOLATE

| Rule | Reason |
|------|--------|
| No `background-color` on `QWidget` globally | Sets `WA_StyledBackground` on every widget → breaks dock area sizing |
| No QSS rules on `QSpinBox` / `QDoubleSpinBox` / `QDateEdit` | Any CSS property forces full CSS paint mode, hiding Fusion's native arrows |
| No `QComboBox::drop-down` subcontrol styling | Makes the dropdown arrow invisible |
| No global `QToolButton { ... }` rules | Inflates toolbar button sizes; scope to `QToolBar QToolButton` only |
| `QMainWindow::separator` must have `padding: 2px` | Required to make it physically grabbable |
| `QGroupBox::title` — no separate `background` or `border` | Title should sit on the group-box border line, not float in a separate box |

## Resource Paths

All resources registered under the `/customstyle` prefix:
- `:/customstyle/stylesheet_dark.qss`
- `:/customstyle/stylesheet_light.qss`
- `:/customstyle/icon_checkbox_checked.png` (and all other icon_*.png)

If you add a new icon, add it to **both**:
1. `SPIERScommon/resources/customstyle/` (the file itself)
2. `SPIERScommon/commonresources.qrc` (the QRC entry under prefix `/customstyle`)

## Adding the Theme Dialog to a New App

SPIERSview / SPIERSalign / SPIERSversion use `AdvancedPreferencesDialog`:
```cpp
#include "advancedpreferencesdialog.h"

void MainWindow::on_actionAdvancedPrefs_triggered() {
    AdvancedPreferencesDialog dlg(this);
    dlg.exec();
}
```
SPIERSedit integrates the theme combo directly into `settings.ui` / `settingsimpl.cpp`.
