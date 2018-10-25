/**
 * @file
 * Header: Semantic Version
 *
 * All SPIERScommon code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2018 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef SemanticVersion_H
#define SemanticVersion_H

#include "prereleasecomponent.h"

#include <QString>
#include <QVector>

/**
 * @brief The SemanticVersion class
 *
 * @see https://semver.org/
 */
class SemanticVersion
{
public:
    SemanticVersion(QVector<int> SemanticVersions, QString preRelease = "", QString metadata = "");

    bool isRelease() const;

    int getSemanticVersion(int index) const;

    QString str() const;
    QString getPreRelease() const;
    QString getMetadata() const;

    static SemanticVersion fromString(QString str);
    SemanticVersion stripPreRelease() const;
    SemanticVersion stripMetadata() const;


    bool operator==(const SemanticVersion &value) const;
    bool operator<(const SemanticVersion &value) const;
    bool operator>(const SemanticVersion &value) const;

private:
    QString metadata;
    QString preRelease;

    QVector<int> versions;
    QVector<PreReleaseComponent> preReleaseComp;

    void parsePreRelease();
    static void matchPreRelease(const QString &str);
    static void matchMetadata(const QString &str);
    static void checkIdentifier(const QString &str);
    static QVector<int> parseSemanticVersion(const QString &str);
};

#endif // SemanticVersion_H
