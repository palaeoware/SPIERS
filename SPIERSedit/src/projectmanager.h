/**
 * @file
 * Header: Project manager singleton class
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

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMutex>

class Project;

/**
 *
 * ProjectManager is a singleton responsible for managing the lifecycle of the
 * current Project. It creates, loads, saves, and closes projects. It provides
 * the primary access point for code to get the current project instance.
 *
 * Thread-safe: Uses QMutex to protect project pointer transitions.
 *
 **/
class ProjectManager : public QObject
{
    Q_OBJECT

public:
    /// Get singleton instance
    static ProjectManager *instance();

    /// Convenience static accessor for current project (may be nullptr)
    static Project *current();

    /// Explicitly set this as parent during construction
    explicit ProjectManager(QObject *parent = nullptr);
    ~ProjectManager();

    // Prevent copying
    ProjectManager(const ProjectManager &) = delete;
    ProjectManager &operator=(const ProjectManager &) = delete;

    // ---- Project Lifecycle ----

    /// Create a new empty project
    bool newProject(const QStringList &imagePaths);

    /// Open an existing project from disk
    bool openProject(const QString &settingsPath);

    /// Save current project (uses existing file path)
    bool saveProject();

    /// Save project to new file path
    bool saveProjectAs(const QString &settingsPath);

    /// Close current project
    bool closeProject();

    // ---- State Queries ----

    /// Check if a project is currently open
    bool hasOpenProject() const;

    /// Get pointer to current project (may be nullptr)
    Project *currentProject() const;

    // ---- Convenience Methods ----

    /// Get current file index (delegates to project, returns -1 if no project)
    int currentFile() const;

    /// Set current file index (delegates to project)
    void setCurrentFile(int index);

    /// Get segment count (delegates to project, returns 0 if no project)
    int segmentCount() const;

signals:
    /// Emitted when a project is successfully opened
    void projectOpened(Project *project);

    /// Emitted when the current project is closed
    void projectClosed();

    /// Emitted when project data changes
    void projectChanged();

private slots:
    /// Internal slot to handle project changed signals
    void onProjectChanged();

private:
    /// Singular static instance
    static ProjectManager *s_instance;

    /// Protect m_currentProject from concurrent access
    mutable QMutex m_projectMutex;

    /// The single open project (nullptr if no project open)
    Project *m_currentProject;

    /// Helper: Set current project and emit signals
    void setCurrentProject(Project *project);

    /// Helper: Delete and clear current project
    void destroyCurrentProject();
};

/// Global convenience function (Qt-style accessor)
inline ProjectManager *projectManager()
{
    return ProjectManager::instance();
}

/// Global convenience function for getting current project
inline Project *currentProject()
{
    auto pm = ProjectManager::instance();
    return pm ? pm->currentProject() : nullptr;
}

#endif // PROJECTMANAGER_H
