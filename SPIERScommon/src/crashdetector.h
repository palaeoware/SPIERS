/**
 * @file
 * Header: Crash Detector
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

#ifndef CRASHDETECTOR_H
#define CRASHDETECTOR_H

#include <QString>

/**
 * @brief The CrashDetector class — installs platform-specific crash handlers
 *
 * This class provides static methods to install unhandled exception handlers
 * across Windows, Linux, and macOS. When a crash is detected, it spawns the
 * SPIERScrashhandler application with crash details.
 */
class CrashDetector
{
public:
    /// Install platform-specific crash handlers (call from main())
    static void installCrashHandlers(const QString &appName);

    /// Trigger a test crash for testing the crash handler (for menu actions)
    static void testCrash(const QString &appName);

    /// Helper to spawn the crash handler process (used by signal handlers)
    static void spawnCrashHandler(const QString &errorMsg, const QString &stackTrace);

    /// Helper to get current stack trace (used by signal handlers)
    static QString getStackTrace();

private:
    /// Store application name for use in crash handlers
    static QString s_appName;
    static QString s_appVersion;
    static int s_crashHandlerInstalled;

    /// Platform-specific installation methods
    static void installWindowsHandler();
    static void installLinuxSignalHandlers();
    static void installMacOSHandler();
    static void installQtMessageHandler();

    /// Helper to capture recent log messages
    static QString getRecentLogs();
};

#endif // CRASHDETECTOR_H
