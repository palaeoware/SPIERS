/**
 * @file
 * Source: Crash Detector
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

#include "crashdetector.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QSysInfo>
#include <QString>
#include <QStandardPaths>
#include <QDir>
#include <QThread>

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <dbghelp.h>
    #pragma comment(lib, "dbghelp.lib")
#endif

#ifdef Q_OS_UNIX
    #include <signal.h>
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/wait.h>
#endif

/// Static member initialization
QString CrashDetector::s_appName;
QString CrashDetector::s_appVersion;
int CrashDetector::s_crashHandlerInstalled = 0;

void CrashDetector::installCrashHandlers(const QString &appName)
{
    if (s_crashHandlerInstalled)
    {
        return;  /// Already installed
    }

    s_appName = appName;

    /// Set version from SOFTWARE_VERSION macro if available
    #ifdef SOFTWARE_VERSION
        s_appVersion = QStringLiteral(SOFTWARE_VERSION);
    #else
        s_appVersion = QStringLiteral("Unknown");
    #endif

    s_crashHandlerInstalled = 1;

    qDebug() << "CrashDetector: Installing crash handlers for" << appName;

    /// Install platform-specific handlers
#ifdef Q_OS_WIN
    installWindowsHandler();
#endif

#ifdef Q_OS_UNIX
    #ifdef Q_OS_MAC
    installMacOSHandler();
    #else
    installLinuxSignalHandlers();
    #endif
#endif

    /// Install Qt message handler as fallback (all platforms)
    installQtMessageHandler();
}

void CrashDetector::testCrash(const QString &appName)
{
    s_appName = appName;

    #ifdef SOFTWARE_VERSION
        s_appVersion = QStringLiteral(SOFTWARE_VERSION);
    #else
        s_appVersion = QStringLiteral("Unknown");
    #endif

    qDebug() << "CrashDetector: Triggering test crash for" << appName;

    /// Intentionally cause a segmentation fault
    volatile int *nullPtr = nullptr;
    *nullPtr = 42;  /// SIGSEGV / Access violation
}

#ifdef Q_OS_WIN

/// Windows exception filter
static LONG WINAPI windowsExceptionFilter(LPEXCEPTION_POINTERS pExceptionInfo)
{
    if (!pExceptionInfo)
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    DWORD exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;
    PVOID exceptionAddr = pExceptionInfo->ExceptionRecord->ExceptionAddress;

    /// Get exception name
    QString exceptionName;
    switch (exceptionCode)
    {
        case EXCEPTION_ACCESS_VIOLATION:
            exceptionName = QStringLiteral("Access Violation");
            break;
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
            exceptionName = QStringLiteral("Array Bounds Exceeded");
            break;
        case EXCEPTION_BREAKPOINT:
            exceptionName = QStringLiteral("Breakpoint");
            break;
        case EXCEPTION_DATATYPE_MISALIGNMENT:
            exceptionName = QStringLiteral("Datatype Misalignment");
            break;
        case EXCEPTION_FLT_DENORMAL_OPERAND:
            exceptionName = QStringLiteral("FLT Denormal Operand");
            break;
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:
            exceptionName = QStringLiteral("FLT Divide by Zero");
            break;
        case EXCEPTION_FLT_INEXACT_RESULT:
            exceptionName = QStringLiteral("FLT Inexact Result");
            break;
        case EXCEPTION_FLT_INVALID_OPERATION:
            exceptionName = QStringLiteral("FLT Invalid Operation");
            break;
        case EXCEPTION_FLT_OVERFLOW:
            exceptionName = QStringLiteral("FLT Overflow");
            break;
        case EXCEPTION_FLT_STACK_CHECK:
            exceptionName = QStringLiteral("FLT Stack Check");
            break;
        case EXCEPTION_FLT_UNDERFLOW:
            exceptionName = QStringLiteral("FLT Underflow");
            break;
        case EXCEPTION_ILLEGAL_INSTRUCTION:
            exceptionName = QStringLiteral("Illegal Instruction");
            break;
        case EXCEPTION_INT_DIVIDE_BY_ZERO:
            exceptionName = QStringLiteral("INT Divide by Zero");
            break;
        case EXCEPTION_INT_OVERFLOW:
            exceptionName = QStringLiteral("INT Overflow");
            break;
        case EXCEPTION_INVALID_DISPOSITION:
            exceptionName = QStringLiteral("Invalid Disposition");
            break;
        case EXCEPTION_NONCONTINUABLE_EXCEPTION:
            exceptionName = QStringLiteral("Noncontinuable Exception");
            break;
        case EXCEPTION_PRIV_INSTRUCTION:
            exceptionName = QStringLiteral("Privileged Instruction");
            break;
        case EXCEPTION_SINGLE_STEP:
            exceptionName = QStringLiteral("Single Step");
            break;
        case EXCEPTION_STACK_OVERFLOW:
            exceptionName = QStringLiteral("Stack Overflow");
            break;
        default:
            exceptionName = QString::asprintf("Exception 0x%08X", exceptionCode);
            break;
    }

    QString errorMsg = exceptionName + QString::asprintf(" (0x%08X) at 0x%p",
                                                         exceptionCode, exceptionAddr);

    CrashDetector::spawnCrashHandler(errorMsg, CrashDetector::getStackTrace());

    return EXCEPTION_EXECUTE_HANDLER;
}

void CrashDetector::installWindowsHandler()
{
    SetUnhandledExceptionFilter(windowsExceptionFilter);
    qDebug() << "CrashDetector: Windows exception handler installed";
}

#else

void CrashDetector::installWindowsHandler()
{
    /// No-op on non-Windows platforms
}

#endif

#ifdef Q_OS_UNIX

/// Signal handler for Unix systems
static void unixSignalHandler(int sig)
{
    QString signalName;
    switch (sig)
    {
        case SIGSEGV:
            signalName = QStringLiteral("Segmentation Fault");
            break;
        case SIGABRT:
            signalName = QStringLiteral("Abort Signal");
            break;
        case SIGTERM:
            signalName = QStringLiteral("Termination Signal");
            break;
        case SIGFPE:
            signalName = QStringLiteral("Floating-Point Exception");
            break;
        case SIGILL:
            signalName = QStringLiteral("Illegal Instruction");
            break;
        case SIGBUS:
            signalName = QStringLiteral("Bus Error");
            break;
        default:
            signalName = QString::asprintf("Signal %d", sig);
            break;
    }

    QString errorMsg = signalName + QString::asprintf(" (signal %d)", sig);
    CrashDetector::spawnCrashHandler(errorMsg, CrashDetector::getStackTrace());

    _exit(128 + sig);  /// Standard exit code for signals
}

void CrashDetector::installLinuxSignalHandlers()
{
    signal(SIGSEGV, unixSignalHandler);
    signal(SIGABRT, unixSignalHandler);
    signal(SIGTERM, unixSignalHandler);
    signal(SIGFPE, unixSignalHandler);
    signal(SIGILL, unixSignalHandler);
    signal(SIGBUS, unixSignalHandler);
    qDebug() << "CrashDetector: Linux signal handlers installed";
}

void CrashDetector::installMacOSHandler()
{
    /// macOS uses the same POSIX signal handlers as Linux
    installLinuxSignalHandlers();
    qDebug() << "CrashDetector: macOS signal handlers installed";
}

#else

void CrashDetector::installLinuxSignalHandlers()
{
    /// No-op on non-Unix platforms
}

void CrashDetector::installMacOSHandler()
{
    /// No-op on non-macOS platforms
}

#endif

/// Qt message handler for fatal Qt messages
static void qtCrashHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static bool inHandler = false;

    if (type == QtFatalMsg && !inHandler)
    {
        inHandler = true;

        QString errorMsg = QStringLiteral("Qt Fatal: ") + msg;
        if (context.file)
        {
            errorMsg += QString::asprintf(" (at %s:%u in %s)", context.file, context.line, context.function);
        }

        CrashDetector::spawnCrashHandler(errorMsg, CrashDetector::getStackTrace());

        inHandler = false;
    }
}

void CrashDetector::installQtMessageHandler()
{
    qInstallMessageHandler(qtCrashHandler);
    qDebug() << "CrashDetector: Qt message handler installed";
}

QString CrashDetector::getStackTrace()
{
    QString trace;

#ifdef Q_OS_WIN
    /// Windows stack trace using dbghelp
    void *addrarray[32];
    unsigned short frames = CaptureStackBackTrace(0, 32, addrarray, nullptr);

    SYMBOL_INFO *symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    HANDLE process = GetCurrentProcess();
    SymInitialize(process, nullptr, TRUE);

    for (unsigned int i = 0; i < frames && i < 20; ++i)
    {
        if (SymFromAddr(process, (DWORD64)addrarray[i], nullptr, symbol))
        {
            trace += QString::asprintf("  #%u %s [0x%llX]\n", i, symbol->Name, symbol->Address);
        }
    }

    free(symbol);
#else
    /// Unix stack trace using backtrace
    void *addrarray[32];
    int numframes = backtrace(addrarray, 32);
    char **symbollist = backtrace_symbols(addrarray, numframes);

    for (int i = 0; i < numframes && i < 20; ++i)
    {
        trace += QStringLiteral("  #") + QString::number(i) + QStringLiteral(" ");

        /// Try to demangle C++ symbols
        char *demangled = nullptr;
        int status = 0;
        demangled = abi::__cxa_demangle(symbollist[i], nullptr, nullptr, &status);

        if (status == 0 && demangled)
        {
            trace += demangled;
            free(demangled);
        }
        else
        {
            trace += symbollist[i];
        }

        trace += QStringLiteral("\n");
    }

    free(symbollist);
#endif

    return trace.isEmpty() ? QStringLiteral("(stack trace not available)") : trace;
}

QString CrashDetector::getRecentLogs()
{
    /// TODO: Implement log capture from qDebug/qWarning/qCritical messages
    /// For now, return empty string - can be enhanced to buffer messages
    return QString();
}

void CrashDetector::spawnCrashHandler(const QString &errorMsg, const QString &stackTrace)
{
    qDebug() << "CrashDetector: Spawning crash handler with error:" << errorMsg;

    /// Build path to crash handler executable
    QString handlerName = QStringLiteral("SPIERScrashhandler64");
#ifdef Q_OS_WIN
    handlerName += QStringLiteral(".exe");
#endif

    QString appDirPath = QCoreApplication::applicationDirPath();
    QString handlerPath = appDirPath + QDir::separator() + handlerName;

    /// Build command-line arguments
    QStringList arguments;
    arguments << QStringLiteral("--app=") + s_appName;
    arguments << QStringLiteral("--version=") + s_appVersion;
    arguments << QStringLiteral("--time=") + QDateTime::currentDateTime().toString();
    arguments << QStringLiteral("--error=") + errorMsg;
    arguments << QStringLiteral("--os-name=") + QSysInfo::productType();
    arguments << QStringLiteral("--os-version=") + QSysInfo::productVersion();
    arguments << QStringLiteral("--arch=") + QSysInfo::buildAbi();
    arguments << QStringLiteral("--qt-version=") + QString(QT_VERSION_STR);
    arguments << QStringLiteral("--cpu-count=") + QString::number(QThread::idealThreadCount());
    arguments << QStringLiteral("--stack=") + stackTrace;

    QString logs = getRecentLogs();
    if (!logs.isEmpty())
    {
        arguments << QStringLiteral("--logs=") + logs;
    }

    /// Try to spawn crash handler process
    bool success = QProcess::startDetached(handlerPath, arguments);
    if (!success)
    {
        qWarning() << "Failed to spawn crash handler:" << handlerPath;
    }
}
