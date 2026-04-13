/**
 * @file
 * Source: Main Entry Point
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

#include <QApplication>
#include <QSysInfo>
#include <QThread>

#include "crashhandler.h"
#include "crashdialog.h"

/**
 * @brief main — SPIERScrashhandler entry point
 *
 * Receives crash information via command-line arguments and displays a crash report dialog
 * with options to copy the report or open GitHub issues.
 *
 * Command-line arguments format:
 *   --app=<name>              Application name (SPIERSedit/SPIERSview/SPIERSalign)
 *   --version=<version>       Application version
 *   --time=<timestamp>        Crash timestamp
 *   --error=<message>         Error message or signal name
 *   --code=<code>             Exception/signal code
 *   --stack=<trace>           Stack trace
 *   --logs=<data>             Recent log messages (newline-separated)
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /// Create crash handler and parse arguments
    CrashHandler handler;
    QStringList args = app.arguments();

    /// Get or construct system information
    CrashHandler::CrashInfo info = handler.parseCrashArguments(args);

    /// Fill in system information if not already provided
    if (info.osName.isEmpty())
    {
        info.osName = QSysInfo::productType();
    }
    if (info.osVersion.isEmpty())
    {
        info.osVersion = QSysInfo::productVersion();
    }
    if (info.architecture.isEmpty())
    {
        info.architecture = QSysInfo::buildAbi();
    }
    if (info.qtVersion.isEmpty())
    {
        info.qtVersion = QStringLiteral(QT_VERSION_STR);
    }
    if (info.cpuCount <= 0)
    {
        info.cpuCount = QThread::idealThreadCount();
    }

    /// Format the crash report
    QString formattedReport = handler.formatCrashReport(info);

    /// Use app name or default to SPIERS
    QString displayAppName = info.appName.isEmpty() ? QStringLiteral("SPIERS") : info.appName;

    /// Create and show the crash dialog
    CrashDialog dialog(displayAppName, formattedReport);
    dialog.exec();

    return 0;
}
