/**
 * @file
 * Header: Crash Handler
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

#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <QString>
#include <QStringList>

/**
 * @brief The CrashHandler class — parses and formats crash report data
 */
class CrashHandler
{
public:
    CrashHandler();

    /// Structure to hold crash report information
    struct CrashInfo
    {
        QString appName;            /// Application name (SPIERSedit/SPIERSview/SPIERSalign)
        QString appVersion;         /// Application version
        QString timestamp;          /// Crash timestamp
        QString osName;             /// Operating system name
        QString osVersion;          /// Operating system version
        QString architecture;       /// System architecture
        QString qtVersion;          /// Qt version
        int cpuCount;               /// CPU core count
        QString errorMessage;       /// Error message or signal name
        QString errorCode;          /// Exception/signal code
        QString stackTrace;         /// Stack trace information
        QStringList recentLogs;     /// Recent log messages
    };

    /// Parse crash information from command-line arguments
    CrashInfo parseCrashArguments(const QStringList &args);

    /// Format crash info into readable report
    QString formatCrashReport(const CrashInfo &info);

    /// Get the application name from crash data
    static QString getApplicationNameForDisplay(const QString &appName);

private:
    /// Helper to parse a command-line argument value
    QString parseArgument(const QStringList &args, const QString &key);

    /// Helper to split multi-line log data
    QStringList parseLogLines(const QString &logData);
};

#endif // CRASHHANDLER_H
