# SPIERS macOS Packaging Scripts

Two professional packaging scripts are provided for creating distribution packages on macOS. Each serves a different purpose in the development and release workflow.

## 📦 Packaging Options

### 1. **`package-macos.sh`** — Production Distribution

Production-ready packages for public release with standalone, user-friendly binaries and no external Qt dependencies.

```bash
./package-macos.sh
```

**Features:**
- ✅ Bundles all Qt6 frameworks into each application using `macdeployqt`
- ✅ Applications run on any macOS system without Qt installation
- ✅ Professional drag-and-drop DMG installer with custom layout
- ✅ Larger file size (~80–150 MB depending on Qt version)
- ✅ Build time: 5–10 minutes (first run may be longer due to Qt framework bundling)

**Output:**
- `bin/SPIERS_vX.X.X_macOS.dmg` — Full standalone installer
- `bin/SPIERS_vX.X.X_macOS.zip` — Full standalone archive

**Recommended for:**
- Official public releases on GitHub
- Distribution to non-technical end users
- App Store submissions
- Ensuring zero Qt dependency issues

---

### 2. **`package-macos-fast.sh`** — Development Builds

Lightweight packaging for rapid iteration during development. Applications require Qt 6 to be pre-installed on the target system.

```bash
./package-macos-fast.sh
```

**Features:**
- ⚡ No Qt framework bundling (target system must have Qt 6 installed)
- ⚡ Significantly faster packaging (~1–2 minutes)
- ⚡ Smaller file size (~20–30 MB)
- ✅ Same professional DMG layout as production script
- ✅ Version automatically extracted from `version.pri`

**Output:**
- `bin/SPIERS_vX.X.X_macOS.dmg` — Lightweight installer
- `bin/SPIERS_vX.X.X_macOS.zip` — Lightweight archive

**Recommended for:**
- Internal team builds and testing
- Quality assurance testing cycles
- Rapid iteration during development
- Teams with Qt 6 pre-installed on all test machines

---

## 🚀 Quick Start

### Public Release Workflow

```bash
# 1. Update version in version.pri if needed
vim version.pri

# 2. Build applications in Release mode
qmake SPIERS.pro && make -j8

# 3. Create standalone distribution packages
./package-macos.sh

# Packages appear in ./bin/ directory
# Ready for GitHub release or distribution
```

### Internal Testing Workflow

```bash
# Fast packaging for quick iterations
./package-macos-fast.sh

# Packages in ./bin/ in ~1–2 minutes
```

---

## 📋 Common Features

Both scripts include the following:

**Professional DMG Installer:**
- Icon arrangement (SPIERSalign, SPIERSedit, SPIERSview)
- Applications folder symlink for drag-to-install workflow
- README.md and LICENSE.md documentation
- Custom window layout and icon positioning via AppleScript

**Cross-Platform .zip Archive:**
- Compatible with macOS, Windows, and Linux systems
- Same directory structure as DMG
- Further compressed for smaller download size

**Intelligent Build Detection:**
- Auto-detects Release or Debug builds
- Version extracted from `version.pri`
- Automatic `macdeployqt` path discovery
- Creates `./bin/` output directory if needed

---

## 🔄 Release Workflow

Standard workflow for releasing a new version:

```bash
# 1. Update version in version.pri
sed -i '' "s/SOFTWARE_VERSION='.*'/SOFTWARE_VERSION='\"X.X.X\"'/" version.pri

# 2. Build Release configuration
qmake SPIERS.pro
make -j8

# 3. Create distribution packages
./package-macos.sh

# 4. Tag and upload to GitHub
git tag vX.X.X
gh release create vX.X.X \
  bin/SPIERS_vX.X.X_macOS.dmg \
  bin/SPIERS_vX.X.X_macOS.zip
```

---

## ⚙️ Requirements

**Both scripts require:**
- macOS command-line tools (via `xcode-select`)
- `hdiutil`, `zip`, `osascript` (standard macOS utilities)
- Compiled SPIERS applications (all three targets)

**Additional requirement for `package-macos.sh`:**
- Qt 6 installation with `macdeployqt` available in PATH or at known locations

---

## 💡 Usage Guidelines

1. Use `package-macos-fast.sh` during development for rapid iteration
2. Use `package-macos.sh` for official releases
3. Both scripts generate identical DMG layouts; the only difference is Qt framework bundling
4. Share `.dmg` files with macOS users for the native experience
5. Share `.zip` files for cross-platform distribution

---

## 🛠️ Troubleshooting

### "macdeployqt not found" error

Occurs when running `package-macos.sh`:
- Verify Qt 6 installation: `ls ~/Qt/6*/macos/bin/macdeployqt`
- If not found, install Qt 6 via Qt Installer or Homebrew
- Update the Qt path search in the script if necessary

### DMG mounting fails

Possible causes during DMG creation:
- Ensure at least 2 GB of free disk space
- Verify `/tmp/` directory has write permissions
- Check for stale mounts: `hdiutil info | grep SPIERS`

### Packaging takes excessive time

If the fast script is too slow:
- Use `package-macos-fast.sh` instead
- Full script bundling large Qt frameworks accounts for longer build time
- Consider parallel build flags: `make -j8` or higher for initial compilation

### Icon not appearing in bundled application

Rare issue after qmake configuration changes:
- Clean build cache: `rm -rf build/`
- Regenerate Makefiles: `qmake SPIERS.pro`
- Rebuild applications: `make -j8`

---

## 📊 Performance Comparison

| Metric | Full (macdeployqt) | Fast (no Qt) |
|--------|-------------------|------------|
| .zip file size | ~80–100 MB | ~15–20 MB |
| .dmg file size | ~50–80 MB | ~10–15 MB |
| Build time | 5–10 min | 1–2 min |
| Qt dependency | Included | Required |
| Use case | Public release | Development |

---

Last Updated: 2026-04-11
