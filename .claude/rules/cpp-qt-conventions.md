# C++ / Qt Coding Conventions (Palaeoware Standard)

Apply these rules to all SPIERS source files. The full specification is at
https://github.com/palaeoware/repoconventions — this file is the working
summary for Claude Code sessions.

---

## File Header (GPL boilerplate)

Every `.h` and `.cpp` must open with:
```cpp
/**
 * @file
 * Header: <Brief one-line description>
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */
```

---

## Naming

| Thing | Convention | Example |
|-------|-----------|---------|
| Classes | Start with uppercase, PascalCase | `MainWindow`, `CustomStyleTheme` |
| Functions & methods | Start with lowercase, camelCase | `applyToApplication()`, `loadFile()` |
| Variables | Start with lowercase, camelCase | `segmentCount`, `fileNameString` |
| Constructors / destructors | Match class name (exception to camelCase rule) | `MainWindow()`, `~MainWindow()` |
| Qt auto-slots | `on_<widget>_<signal>()` | `on_actionAdvancedPrefs_triggered()` |
| Member variables | camelCase (existing code); `m_camelCase` for new code | `m_mode`, `segmentList` |
| Global / compile-time constants | `UPPER_SNAKE` | `SOFTWARE_VERSION` |
| Source files | `lowercasewithoutunderscores.h` / `.cpp` | `customstyletheme.h` |

**Variable declarations:**
- One variable per line — never `int a, b, c;`.
- Avoid short or meaningless names (`tmp`, `x2`, `flag`).
- Single-character names are only acceptable for loop counters (`i`, `j`, `k`) and
  trivial temporaries where the purpose is immediately obvious from context.

---

## Pointers and References

Space between the type and `*` / `&`; no space between that and the variable name:
```cpp
int *pointer;          // correct
QString &reference;    // correct

int* pointer;          // wrong
QString & reference;   // wrong
```

---

## Braces — Non-Attached Style

Opening braces go on a **new line** for all constructs (classes, functions, control flow):
```cpp
void MyClass::myFunction()
{
    if (condition)
    {
        doSomething();
    }
    else
    {
        doSomethingElse();
    }
}
```

---

## Defines, Includes, and Variable Declarations Order

Within any translation unit, follow this sequence:

1. **`#define` macros** — each on its own line, placed after the file header comment.
   Add a blank line after the last `#define` if includes follow.

2. **`#include` files** — project `.h` files first, then Qt headers. One per line.
   Both groups sorted alphabetically. Blank line between the two groups.
   Blank line after the last include if variable declarations follow.

3. **Variable / static declarations** — sorted alphabetically by type. Special-storage
   variables (`static`, `extern`, `constexpr`) before plain variables.

```cpp
// 1. Defines
#define MAX_SEGMENTS 512
#define BUFFER_SIZE  4096

// 2. Includes — project headers first, then Qt, each group alphabetical
#include "customstyletheme.h"
#include "globals.h"

#include <QApplication>
#include <QFile>
#include <QString>

// 3. Declarations — static/special before plain, alphabetical by type
static int instanceCount = 0;
bool initialised = false;
int segmentTotal = 0;
```

---

## Line Length

Soft maximum of **200 characters**. Break long function signatures or call chains
onto multiple lines when they become hard to read:
```cpp
void MyClass::veryLongFunctionName(
    const QString &firstArgument,
    int secondArgument,
    bool thirdArgument)
{
    ...
}
```
If breaking onto multiple lines makes the code *less* readable, keep it on one line.

---

## Comment Style (Doxygen)

**Block comments** for functions, classes, and sections of code:
```cpp
/**
 *
 * This function loads the segment data from disk and populates the internal
 * list. Returns false if the file cannot be opened.
 *
 **/
```

**Inline / variable comments** use the triple-slash form:
```cpp
int segmentCount; /// Total number of active segments in the current project
```

Avoid `//` for anything other than short temporary notes during development.

---

## Header Guards

```cpp
#ifndef MYFILENAME_H
#define MYFILENAME_H
// ...
#endif // MYFILENAME_H
```
Do not use `#pragma once` — the existing codebase uses traditional guards.

---

## Qt Containers and Types

- Prefer `QString` over `std::string`.
- Prefer `QList<T>` / `QVector<T>` over `std::vector<T>`.
- Prefer `QMap<K,V>` over `std::map<K,V>`.
- Use `QStringLiteral("text")` for compile-time string literals.

---

## Qt Patterns

- Every class using signals or slots must have `Q_OBJECT`.
- New-style connect syntax only:
  ```cpp
  connect(source, &Source::signal, destination, &Destination::slot);
  ```
- `QSettings("Palaeoware", "SPIERS")` is the shared cross-app preferences store.
- For `QDialog` subclasses: use `exec()` for modal, `show()` + `setAttribute(Qt::WA_DeleteOnClose)` for non-modal.

---

## Const Correctness

- Mark member functions `const` when they do not mutate state.
- Pass large objects (`QString`, `QVector`, etc.) as `const &`.
- Prefer `const auto &` in range-for loops.

---

## Formatting

- 4-space indentation, no tabs.
- Each app directory has an `.astylerc` for AStyle; run it before committing.
- Follow the non-attached brace style shown above — do not mix styles within a file.

---

## Error Handling

- Always check the return value of `QFile::open()`.
- Connect `finished()` on network replies and inspect `error()`.
- Do not use C++ exceptions — the project does not enable exception handling.

---

## Versioning

All releases follow **Semantic Versioning** (semver.org) compatible with **PEP 440**:

```
[N!]N(.N)*[{a|b|rc}N][.postN][.devN]
```

- Version string is defined in `version.pri` only: `DEFINES += SOFTWARE_VERSION='\\"4.0.0\\"'`
- Release branches are named `origin/vN.N.N` (e.g. `origin/v4.0.0`) — Read the Docs
  parses these automatically.
- Do not hard-code the version string anywhere other than `version.pri`.
