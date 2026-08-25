#!/bin/bash

##
## SPIERS macOS Packaging Script (FAST VERSION - WITHOUT macdeployqt)
## Creates .dmg and .zip without bundling Qt frameworks
## Use this for quick internal releases; use package-macos.sh for distribution
## Usage: ./package-macos-fast.sh
##

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo -e "${YELLOW}=== SPIERS macOS Packaging Script (FAST) ===${NC}"

# Extract version
VERSION=$(grep 'SOFTWARE_VERSION' version.pri | sed 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')

if [ -z "$VERSION" ]; then
    echo -e "${RED}Error: Could not extract version from version.pri${NC}"
    exit 1
fi

echo -e "${GREEN}Version: $VERSION${NC}"

# Define paths
STAGING_DIR="${SCRIPT_DIR}/SPIERS_macOS_staging"
BIN_DIR="${SCRIPT_DIR}/bin"
PACKAGE_NAME="SPIERS_v${VERSION}_macOS"
DMG_FILE="${BIN_DIR}/${PACKAGE_NAME}.dmg"
ZIP_FILE="${BIN_DIR}/${PACKAGE_NAME}.zip"

mkdir -p "$BIN_DIR"

# Find build directory
echo -e "${YELLOW}Searching for build directory...${NC}"
BUILD_DIR=""
if [ -d "${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Release" ]; then
    BUILD_DIR="${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Release"
    echo -e "${GREEN}Found Release build${NC}"
elif [ -d "${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Debug" ]; then
    BUILD_DIR="${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Debug"
    echo -e "${YELLOW}Using Debug build${NC}"
else
    echo -e "${RED}Error: Build directory not found${NC}"
    exit 1
fi

# Check binaries
echo -e "${YELLOW}Checking binaries...${NC}"
for app in "SPIERSalign/bin/SPIERSalign64.app" "SPIERSedit/bin/SPIERSedit64.app" "SPIERSview/bin/SPIERSview64.app" "SPIERScrashhandler/bin/SPIERScrashhandler64.app"; do
    if [ ! -d "${BUILD_DIR}/${app}" ]; then
        echo -e "${RED}Error: Missing ${app}${NC}"
        exit 1
    fi
done
echo -e "${GREEN}All binaries found!${NC}"

# Clean staging
if [ -d "$STAGING_DIR" ]; then
    rm -rf "$STAGING_DIR"
fi

mkdir -p "$STAGING_DIR"

# Copy apps WITHOUT macdeployqt (fast mode)
echo -e "${YELLOW}Copying applications (no Qt bundling)...${NC}"
cp -r "${BUILD_DIR}/SPIERSalign/bin/SPIERSalign64.app" "$STAGING_DIR/SPIERSalign64.app"
cp -r "${BUILD_DIR}/SPIERSedit/bin/SPIERSedit64.app" "$STAGING_DIR/SPIERSedit64.app"
cp -r "${BUILD_DIR}/SPIERSview/bin/SPIERSview64.app" "$STAGING_DIR/SPIERSview64.app"
cp -r "${BUILD_DIR}/SPIERScrashhandler/bin/SPIERScrashhandler64.app" "$STAGING_DIR/SPIERScrashhandler64.app"

# Copy docs
echo -e "${YELLOW}Copying documentation...${NC}"
[ -f "README.md" ] && cp README.md "$STAGING_DIR/README.md"
[ -f "LICENSE.md" ] && cp LICENSE.md "$STAGING_DIR/LICENSE.md"

# Create .zip (FAST) - WITHOUT Applications symlink
echo -e "${YELLOW}Creating .zip...${NC}"
[ -f "$ZIP_FILE" ] && rm "$ZIP_FILE"
cd "$STAGING_DIR"
# Exclude Applications folder from zip (only needed for DMG)
zip -r -q "$ZIP_FILE" . -x "Applications/*"
cd "$SCRIPT_DIR"
echo -e "${GREEN}Created: $ZIP_FILE${NC}"

# Create Applications symlink for DMG (AFTER zipping)
ln -s /Applications "${STAGING_DIR}/Applications"

# Create .dmg (FAST)
echo -e "${YELLOW}Creating .dmg...${NC}"
[ -f "$DMG_FILE" ] && rm "$DMG_FILE"

TEMP_RW_DMG="${SCRIPT_DIR}/${PACKAGE_NAME}_rw.dmg"
hdiutil create -volname "SPIERS $VERSION" -srcfolder "$STAGING_DIR" -ov -format UDRW "$TEMP_RW_DMG" > /dev/null 2>&1

MOUNT_RESULT=$(hdiutil attach "$TEMP_RW_DMG" -readwrite -noautoopen 2>&1)
MOUNT_POINT="/Volumes/SPIERS $VERSION"
sleep 1

# Simple icon layout
osascript <<EOF 2>/dev/null || true
tell application "Finder"
    tell disk "SPIERS $VERSION"
        open
        delay 1
        set current view of container window to icon view
        set icon size of icon view options of container window to 64
        set position of item "SPIERSalign.app" of container window to {80, 100}
        set position of item "SPIERSedit.app" of container window to {180, 100}
        set position of item "SPIERSview.app" of container window to {280, 100}
        set position of item "SPIERScrashhandler.app" of container window to {380, 100}
        set position of item "Applications" of container window to {480, 100}
        close
    end tell
end tell
EOF

sleep 1
hdiutil eject "$MOUNT_POINT" > /dev/null 2>&1 || true

# Convert to compressed
hdiutil convert "$TEMP_RW_DMG" -format UDZO -imagekey zlib-level=9 -o "$DMG_FILE" > /dev/null 2>&1
rm -f "$TEMP_RW_DMG"
echo -e "${GREEN}Created: $DMG_FILE${NC}"

# Cleanup
# Remove the Applications symlink first if it exists
[ -L "${STAGING_DIR}/Applications" ] && unlink "${STAGING_DIR}/Applications"
# Then remove the entire staging directory
rm -rf "$STAGING_DIR"

# Summary
echo ""
echo -e "${GREEN}=== Packaging Complete ===${NC}"
echo -e "Version: ${YELLOW}${VERSION}${NC}"
echo ""
ls -lh "$ZIP_FILE" "$DMG_FILE" | awk '{print "  " $9 " (" $5 ")"}'
echo ""
echo -e "${YELLOW}Note: Apps require Qt 6 to be installed${NC}"
echo -e "${GREEN}Ready for distribution!${NC}"
