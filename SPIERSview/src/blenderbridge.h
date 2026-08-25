/**
 * @file
 * Header: Blender Bridge
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef BLENDERBRIDGE_H
#define BLENDERBRIDGE_H

#include <QString>
#include <QList>

class SVObject;

/**
 *
 * Manages integration with Blender for converting FBX files to .blend format.
 *
 * Provides:
 * - Detection of Blender installation in system PATH
 * - Execution of Blender Python scripts in background mode
 * - Material and hierarchy preservation during FBX → .blend conversion
 * - Error reporting and diagnostics
 *
 * BlenderBridge attempts to locate Blender in standard installation paths
 * and the system PATH. If Blender cannot be found, conversion fails gracefully,
 * allowing fallback to direct FBX export.
 *
 **/
class BlenderBridge
{
public:
    /**
     *
     * Check if Blender is available on the system.
     *
     * Searches system PATH and common installation directories for Blender executable.
     * Results are cached for performance.
     *
     * @return true if Blender found and executable, false otherwise
     *
     **/
    static bool isBlenderAvailable();

    /**
     *
     * Get the full path to Blender executable.
     *
     * Returns path if found, empty string otherwise. Performs detection if not
     * cached. Works on Windows, macOS, and Linux.
     *
     * @return Absolute path to blender (or blender.exe on Windows), or empty string
     *
     **/
    static QString getBlenderExecutable();

    /**
     *
     * Convert FBX file to Blender .blend file with materials applied.
     *
     * Uses Blender's Python API (bpy) to:
     * 1. Import FBX geometry
     * 2. Create Blender materials from object colors
     * 3. Assign materials to objects
     * 4. Set per-material properties (metallic, roughness, alpha)
     * 5. Save final result as .blend file
     *
     * Runs Blender in non-interactive background mode via subprocess.
     * Embedded Python converter script handles all material assignment.
     *
     * @param fbxPath        Path to input FBX file (must exist)
     * @param blendPath      Path where output .blend file will be written
     * @param objects        List of SVObjects with color information
     * @param errorMessage   Output parameter: error description if conversion fails
     * @return true if conversion succeeds, false on any error
     *
     **/
    static bool convertFBXToBlend(
        const QString &fbxPath,
        const QString &blendPath,
        const QList<SVObject*> &objects,
        QString &errorMessage);

    /**
     *
     * Force re-detection of Blender installation.
     *
     * Clears cached Blender path and re-searches system.
     * Useful if user installs Blender after initial detection attempt.
     *
     **/
    static void resetBlenderDetection();

private:
    // Cache for Blender executable path
    static QString s_blenderPath;
    static bool s_blenderDetectionDone;

    /**
     *
     * Detect Blender executable in common locations.
     *
     * Checks:
     * - Windows: Program Files, Program Files (x86), AppData/Local
     * - macOS: /Applications, /usr/local/bin
     * - Linux: /usr/bin, ~/.local/bin, /opt/blender
     * - All platforms: system PATH environment variable
     *
     * @return Full path to blender executable, or empty string if not found
     *
     **/
    static QString detectBlenderInSystemPath();

    /**
     *
     * Generate JSON material definition for Blender converter script.
     *
     * Creates JSON object mapping object names to material properties:
     * {
     *   "ObjectName": {
     *     "color": [r, g, b],
     *     "alpha": 0.5,
     *     "metallic": 0.0,
     *     "roughness": 0.5
     *   }
     * }
     *
     * @param objects List of SVObjects with color and material data
     * @return JSON string suitable for passing to Blender Python script
     *
     **/
    static QString createMaterialsJSON(const QList<SVObject*> &objects);

    /**
     *
     * Extract embedded Blender Python converter script from Qt resources.
     *
     * Script is compiled into application binary via .qrc resource file.
     *
     * @return Python script source code as string
     *
     **/
    static QString getConverterScript();
};

#endif // BLENDERBRIDGE_H
