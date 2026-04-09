/**
 * @file
 * Source: Blender Bridge
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

#include "blenderbridge.h"
#include "svobject.h"
#include <QProcess>
#include <QStandardPaths>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QResource>
#include <QSettings>

// Static member initialization
QString BlenderBridge::s_blenderPath;
bool BlenderBridge::s_blenderDetectionDone = false;

/**
 *
 * Check if Blender is available on the system.
 *
 **/
bool BlenderBridge::isBlenderAvailable()
{
    return !getBlenderExecutable().isEmpty();
}

/**
 *
 * Get the full path to Blender executable.
 *
 * Checks user preference first, then auto-detects in system PATH.
 *
 **/
QString BlenderBridge::getBlenderExecutable()
{
    if (!s_blenderDetectionDone)
    {
        // Check if user has configured a Blender path in settings
        QSettings settings(QStringLiteral("Palaeoware"), QStringLiteral("SPIERS"));
        QString userPath = settings.value(QStringLiteral("BlenderPath"), QString()).toString();

        if (!userPath.isEmpty() && QFile::exists(userPath))
        {
            s_blenderPath = userPath;
        }
        else
        {
            // Auto-detect if no user setting or file doesn't exist
            s_blenderPath = detectBlenderInSystemPath();
        }
        s_blenderDetectionDone = true;
    }
    return s_blenderPath;
}

/**
 *
 * Force re-detection of Blender installation.
 *
 **/
void BlenderBridge::resetBlenderDetection()
{
    s_blenderPath.clear();
    s_blenderDetectionDone = false;
}

/**
 *
 * Detect Blender executable in common locations.
 *
 **/
QString BlenderBridge::detectBlenderInSystemPath()
{
    // List of common Blender executable names
    const QStringList executableNames = {
#ifdef Q_OS_WIN
        "blender.exe"
#else
        "blender"
#endif
    };

    // 1. Check system PATH
    QString pathEnv = qgetenv("PATH");
    QStringList pathDirs = pathEnv.split(
#ifdef Q_OS_WIN
        ";"
#else
        ":"
#endif
    );

    for (const QString &dir : pathDirs) {
        for (const QString &exeName : executableNames) {
            QString fullPath = QDir(dir).absoluteFilePath(exeName);
            QFile f(fullPath);
            if (f.exists()) {
                return fullPath;
            }
        }
    }

    // 2. Check common installation directories
    QStringList commonPaths;

#ifdef Q_OS_WIN
    // Windows installation paths
    QString programFiles = qgetenv("PROGRAMFILES");
    QString programFilesX86 = qgetenv("PROGRAMFILES(X86)");

    if (!programFiles.isEmpty()) {
        commonPaths << programFiles + "/Blender Foundation/Blender"
                   << programFiles + "/Blender";
    }
    if (!programFilesX86.isEmpty()) {
        commonPaths << programFilesX86 + "/Blender Foundation/Blender"
                   << programFilesX86 + "/Blender";
    }

    // AppData/Local for portable versions
    QString appData = qgetenv("LOCALAPPDATA");
    if (!appData.isEmpty()) {
        commonPaths << appData + "/Blender";
    }

#elif defined Q_OS_MAC
    // macOS installation paths
    commonPaths << "/Applications/Blender.app/Contents/MacOS"
               << "/usr/local/bin";

#else
    // Linux installation paths
    commonPaths << "/usr/bin"
               << "/usr/local/bin"
               << QDir::homePath() + "/.local/bin"
               << "/opt/blender"
               << "/opt/blender/bin";
#endif

    for (const QString &dir : commonPaths) {
        for (const QString &exeName : executableNames) {
            QString fullPath = QDir(dir).absoluteFilePath(exeName);
            QFile f(fullPath);
            if (f.exists()) {
                return fullPath;
            }
        }
    }

    return QString();  // Not found
}

/**
 *
 * Convert FBX file to Blender .blend file with materials applied.
 *
 **/
bool BlenderBridge::convertFBXToBlend(
    const QString &fbxPath,
    const QString &blendPath,
    const QList<SVObject*> &objects,
    QString &errorMessage)
{
    // Check Blender availability
    QString blenderExe = getBlenderExecutable();
    if (blenderExe.isEmpty()) {
        errorMessage = "Blender not found. Please install Blender or use FBX export instead.";
        return false;
    }

    // Check FBX file exists
    QFile fbxFile(fbxPath);
    if (!fbxFile.exists()) {
        errorMessage = QString("FBX file not found: %1").arg(fbxPath);
        return false;
    }

    // Create materials JSON
    QString materialsJSON = createMaterialsJSON(objects);

    qDebug() << materialsJSON;

    // Create temporary Python script file
    QString tempScriptPath = blendPath + ".blend_converter.py";
    QString converterScript = getConverterScript();

    QFile scriptFile(tempScriptPath);
    if (!scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        errorMessage = "Failed to create temporary Python script.";
        return false;
    }
    scriptFile.write(converterScript.toUtf8());
    scriptFile.close();

    // Build Blender command
    QStringList arguments;
    arguments << "--background"           // Non-interactive mode
              << "--python" << tempScriptPath  // Run Python script
              << "--"                     // Separator for script arguments
              << fbxPath                  // FBX input path
              << materialsJSON            // Materials as JSON
              << blendPath;               // Output .blend path

    // Execute Blender
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(blenderExe, arguments);

    // Wait for completion (max 60 seconds)
    if (!process.waitForFinished(60000)) {
        process.kill();
        errorMessage = "Blender conversion timed out.";
        QFile::remove(tempScriptPath);
        return false;
    }

    // Check for errors
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    if (process.exitCode() != 0) {
        errorMessage = QString("Blender conversion failed (exit code %1):\n%2")
            .arg(process.exitCode())
            .arg(output);
        QFile::remove(tempScriptPath);
        return false;
    }

    // Verify output file was created
    QFile blendFile(blendPath);
    if (!blendFile.exists()) {
        errorMessage = "Blender did not create output file. Check Blender installation.";
        QFile::remove(tempScriptPath);
        return false;
    }

    // Clean up temporary script
    QFile::remove(tempScriptPath);

    return true;
}

/**
 *
 * Generate JSON material definition for Blender converter script.
 *
 **/
QString BlenderBridge::createMaterialsJSON(const QList<SVObject*> &objects)
{
    QJsonObject materialsObj;

    for (const SVObject *obj : objects) {
        QJsonObject matObj;

        // Color as normalized float array [0-1]
        QJsonArray colorArray;
        colorArray.append(obj->Colour[0] / 255.0);
        colorArray.append(obj->Colour[1] / 255.0);
        colorArray.append(obj->Colour[2] / 255.0);
        matObj["color"] = colorArray;

        // Alpha from transparency code (0-4 → 1.0-0.0)
        float alpha = 1.0f - (obj->Transparency * 0.25f);
        matObj["alpha"] = qMax(0.0, qMin(1.0, (double)alpha));

        // Metallic (all objects non-metallic)
        matObj["metallic"] = 0.0;

        // Roughness from shininess code (0-3 → 0.8-0.0)
        static const float roughnessMap[] = { 0.8f, 0.5f, 0.2f, 0.0f };
        int shininessIdx = qMax(0, qMin(3, obj->Shininess));
        matObj["roughness"] = (double)roughnessMap[shininessIdx];

        materialsObj[obj->Name] = matObj;
    }

    QJsonDocument doc(materialsObj);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

/**
 *
 * Extract embedded Blender Python converter script from Qt resources.
 *
 **/
QString BlenderBridge::getConverterScript()
{
    // Read from Qt resource file
    QFile scriptRes(":/blender/blender_converter.py");
    if (scriptRes.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString script = QString::fromUtf8(scriptRes.readAll());
        scriptRes.close();
        return script;
    }

    return "";
}
