/**
 * @file
 * Source: Crash Handler
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

#include "crashhandler.h"

#include <QDateTime>
#include <QSysInfo>

CrashHandler::CrashHandler()
{
}

CrashHandler::CrashInfo CrashHandler::parseCrashArguments(const QStringList &args)
{
    CrashInfo info;

    /// Parse standard crash information
    info.appName = parseArgument(args, "--app");
    info.appVersion = parseArgument(args, "--version");
    info.timestamp = parseArgument(args, "--time");
    info.osName = parseArgument(args, "--os-name");
    info.osVersion = parseArgument(args, "--os-version");
    info.architecture = parseArgument(args, "--arch");
    info.qtVersion = parseArgument(args, "--qt-version");
    info.errorMessage = parseArgument(args, "--error");
    info.errorCode = parseArgument(args, "--code");
    info.stackTrace = parseArgument(args, "--stack");

    /// Parse CPU count
    bool ok = false;
    info.cpuCount = parseArgument(args, "--cpu-count").toInt(&ok);
    if (!ok)
    {
        info.cpuCount = -1;
    }

    /// Parse log messages (can be multi-line)
    QString logData = parseArgument(args, "--logs");
    info.recentLogs = parseLogLines(logData);

    return info;
}

QString CrashHandler::formatCrashReport(const CrashInfo &info)
{
    QString report;

    /// Header
    report += QStringLiteral("SPIERS CRASH REPORT\n");
    report += QStringLiteral("===================\n\n");

    /// Application information
    report += QStringLiteral("Application: ") + getApplicationNameForDisplay(info.appName) + QStringLiteral("\n");
    report += QStringLiteral("Version: ") + info.appVersion + QStringLiteral("\n");
    report += QStringLiteral("Time: ") + (info.timestamp.isEmpty() ? QDateTime::currentDateTime().toString() : info.timestamp) + QStringLiteral("\n\n");

    /// System information
    report += QStringLiteral("System Information:\n");
    report += QStringLiteral("  OS: ") + info.osName + (info.osVersion.isEmpty() ? QString() : QStringLiteral(" ") + info.osVersion) + QStringLiteral("\n");
    report += QStringLiteral("  Architecture: ") + (info.architecture.isEmpty() ? QStringLiteral("Unknown") : info.architecture) + QStringLiteral("\n");
    report += QStringLiteral("  Qt Version: ") + (info.qtVersion.isEmpty() ? QStringLiteral("Unknown") : info.qtVersion) + QStringLiteral("\n");
    if (info.cpuCount > 0)
    {
        report += QStringLiteral("  CPU Cores: ") + QString::number(info.cpuCount) + QStringLiteral("\n");
    }
    report += QStringLiteral("\n");

    /// Crash information
    report += QStringLiteral("Crash Information:\n");
    report += QStringLiteral("  Error: ") + (info.errorMessage.isEmpty() ? QStringLiteral("Unknown") : info.errorMessage) + QStringLiteral("\n");
    if (!info.errorCode.isEmpty())
    {
        report += QStringLiteral("  Code: ") + info.errorCode + QStringLiteral("\n");
    }
    report += QStringLiteral("\n");

    /// Stack trace
    if (!info.stackTrace.isEmpty())
    {
        report += QStringLiteral("Stack Trace:\n");
        report += info.stackTrace + QStringLiteral("\n\n");
    }

    /// Recent logs
    if (!info.recentLogs.isEmpty())
    {
        report += QStringLiteral("Recent Log Messages:\n");
        for (const QString &log : info.recentLogs)
        {
            report += QStringLiteral("  ") + log + QStringLiteral("\n");
        }
        report += QStringLiteral("\n");
    }

    /// Footer with GitHub instructions
    report += QStringLiteral("How to Report:\n");
    report += QStringLiteral("Please report this crash to help us improve SPIERS:\n");
    report += QStringLiteral("1. Copy this report to your clipboard (button above)\n");
    report += QStringLiteral("2. Click 'Open GitHub Issues' to go to our issue tracker\n");
    report += QStringLiteral("3. Create a new issue and paste this report\n");

    return report;
}

QString CrashHandler::getApplicationNameForDisplay(const QString &appName)
{
    if (appName == QStringLiteral("SPIERSedit"))
        return QStringLiteral("SPIERSedit");
    else if (appName == QStringLiteral("SPIERSview"))
        return QStringLiteral("SPIERSview");
    else if (appName == QStringLiteral("SPIERSalign"))
        return QStringLiteral("SPIERSalign");
    else
        return appName.isEmpty() ? QStringLiteral("SPIERS Application") : appName;
}

QString CrashHandler::parseArgument(const QStringList &args, const QString &key)
{
    const QString keyPrefix = key + QStringLiteral("=");

    for (int i = 0; i < args.count(); ++i)
    {
        const QString &arg = args[i];

        /// Check for key=value format
        if (arg.startsWith(keyPrefix))
        {
            return arg.mid(keyPrefix.length());
        }

        /// Also check for separate key value format for compatibility
        if (arg == key && i + 1 < args.count())
        {
            return args[i + 1];
        }
    }
    return QString();
}

QStringList CrashHandler::parseLogLines(const QString &logData)
{
    if (logData.isEmpty())
        return QStringList();

    /// Split by newline, limit to last 50 lines
    QStringList lines = logData.split(QStringLiteral("\n"), Qt::SkipEmptyParts);
    if (lines.count() > 50)
    {
        lines = lines.mid(lines.count() - 50);
    }
    return lines;
}
