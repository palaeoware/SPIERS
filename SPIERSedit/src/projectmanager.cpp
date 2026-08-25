/**
 * @file
 * Implementation: Project manager singleton class
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

#include "projectmanager.h"
#include "project.h"

#include <QMutexLocker>

// Static instance initialization
ProjectManager *ProjectManager::s_instance = nullptr;

/**
 *
 * Get singleton instance (creates on first call).
 *
 **/
ProjectManager *ProjectManager::instance()
{
    if (s_instance == nullptr)
    {
        s_instance = new ProjectManager();
    }
    return s_instance;
}

/**
 *
 * Convenience static accessor for current project.
 *
 **/
Project *ProjectManager::current()
{
    ProjectManager *pm = instance();
    return pm ? pm->currentProject() : nullptr;
}

/**
 *
 * Constructor: Initialize manager with no project open.
 *
 **/
ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent), m_currentProject(nullptr)
{
    // Ensure this is the singleton instance
    if (s_instance == nullptr)
    {
        s_instance = this;
    }
}

/**
 *
 * Destructor: Clean up current project if open.
 *
 **/
ProjectManager::~ProjectManager()
{
    if (m_currentProject != nullptr)
    {
        closeProject();
    }
    if (s_instance == this)
    {
        s_instance = nullptr;
    }
}

// ---- Project Lifecycle ----

/**
 *
 * Create a new empty project from a list of image file paths.
 * Closes any existing project first.
 *
 **/
bool ProjectManager::newProject(const QStringList &imagePaths)
{
    // Close existing project if open
    if (m_currentProject != nullptr)
    {
        closeProject();
    }

    // Create new project
    Project *newProject = new Project(this);

    // Set up file lists
    newProject->setFiles(imagePaths);
    QStringList fullFiles;
    for (const QString &file : imagePaths)
    {
        fullFiles.append(file);  // TODO: Convert to full path if needed
    }
    newProject->setFullFiles(fullFiles);

    // Initialize file dirty flags (one per file)
    QList<bool> dirtyFlags;
    for (int i = 0; i < imagePaths.count(); ++i)
    {
        dirtyFlags.append(false);
    }
    // TODO: Set dirty flags in project (need accessor for list)

    // Set as current project
    setCurrentProject(newProject);

    emit projectOpened(newProject);
    return true;
}

/**
 *
 * Open an existing project from disk.
 * Closes any existing project first.
 *
 **/
bool ProjectManager::openProject(const QString &settingsPath)
{
    // Close existing project if open
    if (m_currentProject != nullptr)
    {
        closeProject();
    }

    // Create new project
    Project *newProject = new Project(this);

    // TODO: Load project data from disk using FileIO module
    // For now, just validate path exists
    if (settingsPath.isEmpty())
    {
        delete newProject;
        return false;
    }

    newProject->setSettingsFileName(settingsPath);
    newProject->setFullSettingsFileName(settingsPath);

    // Set as current project
    setCurrentProject(newProject);

    emit projectOpened(newProject);
    return true;
}

/**
 *
 * Save current project to disk (uses existing file path).
 *
 **/
bool ProjectManager::saveProject()
{
    QMutexLocker locker(&m_projectMutex);

    if (m_currentProject == nullptr)
    {
        return false;
    }

    // TODO: Save project data to disk using FileIO module
    // For now, just return success
    return true;
}

/**
 *
 * Save project to new file path.
 *
 **/
bool ProjectManager::saveProjectAs(const QString &settingsPath)
{
    QMutexLocker locker(&m_projectMutex);

    if (m_currentProject == nullptr)
    {
        return false;
    }

    if (settingsPath.isEmpty())
    {
        return false;
    }

    m_currentProject->setSettingsFileName(settingsPath);
    m_currentProject->setFullSettingsFileName(settingsPath);

    // TODO: Save project data to disk using FileIO module
    return true;
}

/**
 *
 * Close current project and clean up resources.
 *
 **/
bool ProjectManager::closeProject()
{
    QMutexLocker locker(&m_projectMutex);

    if (m_currentProject == nullptr)
    {
        return false;
    }

    // TODO: Save current state before closing (if auto-save enabled)

    destroyCurrentProject();

    emit projectClosed();
    return true;
}

// ---- State Queries ----

/**
 *
 * Check if a project is currently open.
 *
 **/
bool ProjectManager::hasOpenProject() const
{
    QMutexLocker locker(&m_projectMutex);
    return m_currentProject != nullptr;
}

/**
 *
 * Get pointer to current project (may be nullptr if no project open).
 *
 **/
Project *ProjectManager::currentProject() const
{
    QMutexLocker locker(&m_projectMutex);
    return m_currentProject;
}

// ---- Convenience Methods ----

/**
 *
 * Get current file index (delegates to project).
 *
 **/
int ProjectManager::currentFile() const
{
    QMutexLocker locker(&m_projectMutex);

    if (m_currentProject == nullptr)
    {
        return -1;
    }

    return m_currentProject->currentFile();
}

/**
 *
 * Set current file index (delegates to project).
 *
 **/
void ProjectManager::setCurrentFile(int index)
{
    QMutexLocker locker(&m_projectMutex);

    if (m_currentProject != nullptr)
    {
        m_currentProject->setCurrentFile(index);
    }
}

/**
 *
 * Get segment count (delegates to project).
 *
 **/
int ProjectManager::segmentCount() const
{
    QMutexLocker locker(&m_projectMutex);

    if (m_currentProject == nullptr)
    {
        return 0;
    }

    return m_currentProject->segmentCount();
}

// ---- Private Helpers ----

/**
 *
 * Set current project and emit connected signals.
 * Release lock before emitting to prevent deadlocks.
 *
 **/
void ProjectManager::setCurrentProject(Project *project)
{
    {
        QMutexLocker locker(&m_projectMutex);
        m_currentProject = project;

        // Connect project's signals to our signals
        if (m_currentProject != nullptr)
        {
            connect(m_currentProject, &Project::projectChanged,
                    this, &ProjectManager::onProjectChanged);
        }
    }

    // Signals emitted without lock held
    if (project != nullptr)
    {
        emit projectOpened(project);
    }
}

/**
 *
 * Delete and clear current project.
 *
 **/
void ProjectManager::destroyCurrentProject()
{
    if (m_currentProject != nullptr)
    {
        disconnect(m_currentProject, nullptr, this, nullptr);
        m_currentProject->deleteLater();  // Safe async delete
        m_currentProject = nullptr;
    }
}

/**
 *
 * Handle project changed signal from current project.
 *
 **/
void ProjectManager::onProjectChanged()
{
    emit projectChanged();
}
