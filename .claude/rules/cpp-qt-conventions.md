# C++ / Qt Coding Conventions

Apply these rules to all SPIERS source files.

## File Headers

Every `.h` and `.cpp` must start with the GPL boilerplate block:
```cpp
/**
 * @file
 * Header: <Brief one-line description>
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2019 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */
```

## Naming

| Thing | Convention | Example |
|-------|-----------|---------|
| Classes | PascalCase | `MainWindow`, `CustomStyleTheme` |
| Source files | lowercase_underscores | `customstyletheme.h` |
| Public/protected methods | camelCase | `applyToApplication()` |
| Qt auto-slots | `on_<widget>_<signal>()` | `on_actionAdvancedPrefs_triggered()` |
| Member variables | `m_camelCase` (new code) | `m_mode` |
| Global constants | `UPPER_SNAKE` | `SOFTWARE_VERSION` |
| Enums (Qt5-style) | PascalCase members | `ThemeMode::Dark` |

## Qt Containers & Types

- Prefer `QString` over `std::string`.
- Prefer `QList<T>` / `QVector<T>` over `std::vector<T>`.
- Prefer `QMap<K,V>` over `std::map<K,V>`.
- Use `QStringLiteral("text")` for string literals that don't change at runtime.

## Qt Patterns

- Every class that emits or receives signals must have `Q_OBJECT` macro.
- Use new-style connect syntax:
  ```cpp
  connect(src, &Source::signal, dst, &Destination::slot);
  ```
- `QSettings("Palaeoware", "SPIERS")` is the shared cross-app store — use it for any preference that should persist across sessions or sync between the four apps.
- For `QDialog` subclasses, call `setAttribute(Qt::WA_DeleteOnClose)` when opened with `show()` (non-modal). Use `exec()` for modal dialogs.

## Const Correctness

- Mark member functions `const` if they don't mutate state.
- Pass large objects (`QString`, `QVector`, etc.) by `const &` to functions.
- Prefer `const auto &` in range-for loops.

## Include Order

```cpp
#include "myclass.h"          // Own header first

#include <QWidget>            // Qt headers
#include <QSettings>
#include <QFile>

#include <algorithm>          // STL (if needed)
```

## Header Guards

```cpp
#ifndef MYFILENAME_H
#define MYFILENAME_H
// content
#endif // MYFILENAME_H
```
Do not use `#pragma once` — the existing codebase uses traditional guards.

## Formatting

- 4-space indentation (no tabs).
- AStyle is used for formatting; each app directory has an `.astylerc`.
- Opening brace on same line for control flow, new line for class/function definitions — follow the style already present in the file being edited.

## Error Handling

- Qt file I/O: always check `QFile::open()` return value.
- Network replies: connect `finished()` and check `error()`.
- Do not use exceptions — Qt code in this project does not use exception handling.
