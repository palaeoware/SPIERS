#!/bin/bash

##
## SPIERS macOS Packaging Script
## This script creates distribution packages (.dmg and .zip) for all three SPIERS applications
## It uses macdeployqt to bundle Qt frameworks and creates a professional drag-and-drop DMG
## Usage: ./package-macos.sh
##

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

echo -e "${YELLOW}=== SPIERS macOS Packaging Script ===${NC}"

# Extract version from version.pri
# Format: DEFINES += SOFTWARE_VERSION='\"X.X.X\"'
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

# Create bin directory if it doesn't exist
if [ ! -d "$BIN_DIR" ]; then
    mkdir -p "$BIN_DIR"
fi

# Find build directory (prefer Release, fall back to Debug)
echo -e "${YELLOW}Searching for build directory...${NC}"
BUILD_DIR=""
if [ -d "${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Release" ]; then
    BUILD_DIR="${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Release"
    echo -e "${GREEN}Found Release build directory${NC}"
elif [ -d "${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Debug" ]; then
    BUILD_DIR="${SCRIPT_DIR}/build/Qt_6_11_0_for_macOS-Debug"
    echo -e "${YELLOW}Using Debug build directory (Release not found)${NC}"
else
    echo -e "${RED}Error: Could not find build directory${NC}"
    echo -e "${YELLOW}Expected: build/Qt_6_11_0_for_macOS-Release or build/Qt_6_11_0_for_macOS-Debug${NC}"
    exit 1
fi

# Find macdeployqt
echo -e "${YELLOW}Locating macdeployqt...${NC}"
MACDEPLOYQT=$(which macdeployqt 2>/dev/null || echo "")
if [ -z "$MACDEPLOYQT" ]; then
    # Try common Qt installation paths
    for QT_PATH in "/Users/alanspencer/Qt/6.11.0/macos/bin/macdeployqt" \
                   "/usr/local/opt/qt6/libexec/macdeployqt" \
                   "/opt/homebrew/opt/qt6/libexec/macdeployqt"; do
        if [ -f "$QT_PATH" ]; then
            MACDEPLOYQT="$QT_PATH"
            break
        fi
    done
fi
if [ -z "$MACDEPLOYQT" ] || [ ! -f "$MACDEPLOYQT" ]; then
    echo -e "${RED}Error: macdeployqt not found${NC}"
    echo -e "${YELLOW}Please ensure Qt 6 is installed${NC}"
    exit 1
fi
echo -e "${GREEN}Found macdeployqt at: $MACDEPLOYQT${NC}"

# Check if binaries exist
echo -e "${YELLOW}Checking for compiled binaries...${NC}"

APPS=(
    "SPIERSalign/bin/SPIERSalign64.app"
    "SPIERSedit/bin/SPIERSedit64.app"
    "SPIERSview/bin/SPIERSview64.app"
    "SPIERScrashhandler/bin/SPIERScrashhandler64.app"
)

for app in "${APPS[@]}"; do
    if [ ! -d "${BUILD_DIR}/${app}" ]; then
        echo -e "${RED}Error: Missing ${BUILD_DIR}/${app}${NC}"
        echo -e "${YELLOW}Please build all three applications first:${NC}"
        echo "  qmake SPIERS.pro && make -j8"
        exit 1
    fi
done

echo -e "${GREEN}All binaries found!${NC}"

# Clean up any previous staging directory
if [ -d "$STAGING_DIR" ]; then
    echo -e "${YELLOW}Cleaning previous staging directory...${NC}"
    rm -rf "$STAGING_DIR"
fi

# Create staging directory
echo -e "${YELLOW}Creating staging directory...${NC}"
mkdir -p "$STAGING_DIR"

# Deploy and copy applications
echo -e "${YELLOW}Deploying applications with Qt frameworks...${NC}"

# Copy each app and run macdeployqt on it
for app_name in "SPIERSalign" "SPIERSedit" "SPIERSview" "SPIERScrashhandler"; do
    SRC_APP="${BUILD_DIR}/${app_name}/bin/${app_name}64.app"
    DEST_APP="${STAGING_DIR}/${app_name}64.app"

    echo -e "${YELLOW}  Processing ${app_name}64...${NC}"
    cp -r "$SRC_APP" "$DEST_APP"

    # Run macdeployqt to bundle Qt frameworks
    "$MACDEPLOYQT" "$DEST_APP" -always-overwrite > /dev/null 2>&1 || true
done

# Copy README and license to staging directory
echo -e "${YELLOW}Copying documentation...${NC}"
if [ -f "README.md" ]; then
    cp README.md "$STAGING_DIR/README.md"
fi

if [ -f "LICENSE.md" ]; then
    cp LICENSE.md "$STAGING_DIR/LICENSE.md"
fi

# Create .zip file (BEFORE adding Applications symlink)
echo -e "${YELLOW}Creating .zip file...${NC}"
if [ -f "$ZIP_FILE" ]; then
    rm "$ZIP_FILE"
fi

cd "$STAGING_DIR"
# Zip without Applications symlink (it's only needed for DMG)
zip -r -q "$ZIP_FILE" . -x "Applications/*"
cd "$SCRIPT_DIR"

echo -e "${GREEN}Created: $ZIP_FILE${NC}"

# Create Applications folder symlink for DMG (AFTER zipping)
echo -e "${YELLOW}Creating Applications folder reference for DMG...${NC}"
ln -s /Applications "${STAGING_DIR}/Applications"

# Create professional .dmg file with drag-and-drop installer
echo -e "${YELLOW}Creating professional .dmg installer...${NC}"
if [ -f "$DMG_FILE" ]; then
    rm "$DMG_FILE"
fi

# Create temporary RW DMG
TEMP_RW_DMG="${SCRIPT_DIR}/${PACKAGE_NAME}_rw.dmg"
TEMP_MOUNT_DIR="/tmp/spiers_dmg_mount_$$"

# Create the DMG
echo -e "${YELLOW}  Creating DMG structure...${NC}"
hdiutil create -volname "SPIERS $VERSION" -srcfolder "$STAGING_DIR" -ov -format UDRW "$TEMP_RW_DMG" > /dev/null 2>&1

# Mount the DMG to customize it
echo -e "${YELLOW}  Customizing DMG layout...${NC}"
MOUNT_RESULT=$(hdiutil attach "$TEMP_RW_DMG" -readwrite -noautoopen 2>&1)
MOUNT_POINT=$(echo "$MOUNT_RESULT" | grep -oE '/Volumes/SPIERS [0-9]+\.[0-9]+\.[0-9]+' | head -1)

if [ -z "$MOUNT_POINT" ]; then
    MOUNT_POINT="/Volumes/SPIERS $VERSION"
fi

# Wait for mount
sleep 1

# Set up icon layout for the DMG using AppleScript
osascript <<EOF 2>/dev/null || true
tell application "Finder"
    tell disk "SPIERS $VERSION"
        open
        delay 1

        -- Set window properties
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false

        -- Set view options
        set the bounds of container window to {100, 100, 600, 500}
        set icon size of icon view options of container window to 64
        set text size of icon view options of container window to 12
        set arrangement of icon view options of container window to arranged by name

        -- Position items
        set position of item "SPIERSalign.app" of container window to {80, 100}
        set position of item "SPIERSedit.app" of container window to {180, 100}
        set position of item "SPIERSview.app" of container window to {280, 100}
        set position of item "SPIERScrashhandler.app" of container window to {380, 100}
        set position of item "Applications" of container window to {480, 100}
        set position of item "README.md" of container window to {100, 250}
        set position of item "LICENSE.md" of container window to {200, 250}

        close
        open
        update without registering applications
        delay 1
        close
    end tell
end tell
EOF

# Eject the DMG
sleep 1
hdiutil eject "$MOUNT_POINT" > /dev/null 2>&1 || true

# Convert to compressed read-only format
echo -e "${YELLOW}  Compressing DMG...${NC}"
hdiutil convert "$TEMP_RW_DMG" -format UDZO -imagekey zlib-level=9 -o "$DMG_FILE" > /dev/null 2>&1

# Clean up temporary DMG
rm -f "$TEMP_RW_DMG"

echo -e "${GREEN}Created: $DMG_FILE${NC}"

# Clean up staging directory
echo -e "${YELLOW}Cleaning up...${NC}"
# Remove the Applications symlink first if it exists
[ -L "${STAGING_DIR}/Applications" ] && unlink "${STAGING_DIR}/Applications"
# Then remove the entire staging directory
rm -rf "$STAGING_DIR"

# Print summary
echo ""
echo -e "${GREEN}=== Packaging Complete ===${NC}"
echo -e "Package Name: ${YELLOW}${PACKAGE_NAME}${NC}"
echo -e "Version: ${YELLOW}${VERSION}${NC}"
echo ""
echo -e "Created packages in: ${YELLOW}${BIN_DIR}/${NC}"
echo ""
echo -e "Files:"
echo -e "  ${YELLOW}${ZIP_FILE}${NC}"
echo -e "  ${YELLOW}${DMG_FILE}${NC}"
echo ""
echo -e "File sizes:"
ls -lh "$ZIP_FILE" "$DMG_FILE" 2>/dev/null | awk '{print "  " $9 " (" $5 ")"}'
echo ""
echo -e "${GREEN}✓ Ready for distribution!${NC}"
echo ""
echo -e "${YELLOW}DMG Features:${NC}"
echo "  • Drag-and-drop installer to /Applications"
echo "  • All Qt frameworks bundled (no external dependencies)"
echo "  • Professional icon layout and arrangement"
echo ""
