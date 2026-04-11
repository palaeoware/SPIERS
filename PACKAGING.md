# SPIERS macOS Packaging Guide

## Overview

The `package-macos.sh` and `package-macos-fast.sh` scripts automate creation of distribution-ready packages for SPIERS on macOS. Both generate `.dmg` (disk image) and `.zip` archive formats containing all three applications: SPIERSalign, SPIERSedit, and SPIERSview.

For a comparison of the two scripts and their appropriate use cases, see [PACKAGING_SCRIPTS.md](PACKAGING_SCRIPTS.md).

## Prerequisites

- All three applications must be successfully compiled
- macOS development tools: `hdiutil`, `zip`, `osascript` (standard system utilities)
- Version in `version.pri` should be updated before packaging
- Qt 6 installation with `macdeployqt` (required for `package-macos.sh` only)

## Building Applications

### Command Line

Ensure all three applications are built in Release mode:

```bash
cd /path/to/SPIERS
qmake SPIERS.pro
make -j8
```

### Qt Creator

1. Select **Release** configuration from the kit selector
2. Navigate to **Build → Run qmake**
3. Navigate to **Build → Build All**

The build system will compile all three applications and place binaries in `SPIERSalign/bin/`, `SPIERSedit/bin/`, and `SPIERSview/bin/`.

## Running the Packaging Script

From the SPIERS project root directory:

```bash
./package-macos.sh
```

The script will:
- Extract version from `version.pri`
- Verify all binary files exist
- Create a temporary staging directory with all three applications
- Run `macdeployqt` on each application to bundle Qt frameworks
- Generate `SPIERS_vX.X.X_macOS.zip`
- Generate `SPIERS_vX.X.X_macOS.dmg` with professional layout
- Display file sizes and completion summary
- Clean up temporary staging files

## Version Management

Version numbers are sourced from `version.pri` using `DEFINES`:

```qmake
DEFINES += SOFTWARE_VERSION='\\"4.0.1\\"'
```

### Release Process

1. Update the version string in `version.pri`
2. Commit the version change
3. Tag the commit with the corresponding version
4. Run the packaging script
5. Upload generated packages to GitHub releases

Example:

```bash
# Update version.pri to 4.0.1
git commit -am "Release v4.0.1"
git tag v4.0.1
./package-macos.sh
gh release create v4.0.1 bin/SPIERS_v4.0.1_macOS.dmg bin/SPIERS_v4.0.1_macOS.zip
```

## Output Files

Both packaging scripts create output in the `bin/` directory:

- `bin/SPIERS_vX.X.X_macOS.zip` — Compressed archive format
- `bin/SPIERS_vX.X.X_macOS.dmg` — macOS disk image format

### DMG Format

Native macOS distribution format providing:
- Familiar installation experience for macOS users
- Professional drag-and-drop installer interface
- Custom window layout with application icons positioned by the script
- Applications folder symlink for easy drag-to-install workflow

### ZIP Format

Universal archive format providing:
- Compatibility across macOS, Windows, and Linux systems
- Smaller file size due to additional compression
- Simple extraction process on any platform
- Identical directory structure to DMG contents

## Troubleshooting

### "Error: Could not extract version from version.pri"

Verify `version.pri` exists in the project root and contains a properly formatted `SOFTWARE_VERSION` definition:

```qmake
DEFINES += SOFTWARE_VERSION='\\"4.0.1\\"'
```

### "Error: Missing SPIERSalign64.app" or similar binaries

Ensure the Release configuration has been compiled:

```bash
qmake SPIERS.pro
make -j8
```

Check the build directory path matches the script's expectations:
```
build/Qt_6_11_0_for_macOS-Release/
```

### DMG creation fails or takes excessive time

- Ensure at least 500 MB of free disk space available
- Verify `/tmp` directory has write permissions
- Check for stale mount points: `hdiutil info | grep SPIERS`
- Consider using `package-macos-fast.sh` for faster iteration during development

### "macdeployqt not found" error

The `package-macos.sh` script searches common Qt installation paths. If not found:

```bash
# Locate macdeployqt
ls ~/Qt/6*/macos/bin/macdeployqt
which macdeployqt
```

Update the Qt path in the script if the installation is in a non-standard location.

## Advanced Customization

### Modifying DMG Appearance

The script uses AppleScript to customize DMG layout. To further customize the appearance:

```bash
# Mount the generated DMG
hdiutil attach bin/SPIERS_vX.X.X_macOS.dmg

# Edit via Finder or additional AppleScript
# Background images, custom positioning, etc.

# Eject when complete
hdiutil eject "/Volumes/SPIERS X.X.X"
```

### Code Signing

For distribution outside the App Store, applications can be code signed:

```bash
# Sign all three applications
codesign --deep --force --verify --verbose \
  --sign "Developer ID Application: Organization Name" \
  bin/SPIERS_vX.X.X_macOS/SPIERSalign64.app \
  bin/SPIERS_vX.X.X_macOS/SPIERSedit64.app \
  bin/SPIERS_vX.X.X_macOS/SPIERSview64.app
```

### Notarization for Gatekeeper

macOS Gatekeeper requires notarization for applications distributed outside App Store. Notarize the `.dmg`:

```bash
xcrun altool --notarize-app \
  -f bin/SPIERS_vX.X.X_macOS.dmg \
  -t osx \
  --file-type dmg \
  --primary-bundle-id com.palaeoware.spiers
```

Monitor notarization status:

```bash
xcrun altool --notarization-info <REQUEST-UUID> -u <APPLE-ID> -p <APP-PASSWORD>
```

## CI/CD Integration

The packaging script integrates with continuous integration pipelines:

```yaml
# Example GitHub Actions workflow
- name: Build SPIERS for macOS
  run: |
    qmake SPIERS.pro
    make -j8
    ./package-macos.sh

- name: Upload to release
  uses: softprops/action-gh-release@v1
  with:
    files: |
      bin/SPIERS_v*.dmg
      bin/SPIERS_v*.zip
```

## References

- Project repository: https://github.com/palaeoware/SPIERS
- Documentation: https://spiers-software.org/
- Qt deployment documentation: https://doc.qt.io/qt-6/macos-deployment.html

---

Last Updated: 2026-04-11
