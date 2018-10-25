/**
 * @file
 * Semantic Version
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

#include "semanticversion.h"

#include <QRegularExpression>
#include <QDebug>

static const QString PRERELEASE_PATTERN("([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)");
static const QString METADATA_PATTERN("([0-9A-Za-z-]+(\\.[0-9A-Za-z-]+)*)");
static const QString VERSION_PATTERN("(0|[1-9][[:digit:]]*)(\\.(0|[1-9][[:digit:]]*)){1,}");

/**
 * @brief SemanticVersion::SemanticVersion
 * @param v
 * @param pr
 * @param md
 */
SemanticVersion::SemanticVersion(QVector<int> v, QString pr, QString md)
{
    versions = v;
    preRelease = pr;
    metadata = md;

    if (this->versions.size() < 2)
    {
        qDebug() << "Semantic Version must contain at least two numbers.";
        throw;
    }

    if (!preRelease.isEmpty())
    {
        matchPreRelease(preRelease);
        parsePreRelease();
    }

    if (!metadata.isEmpty())
    {
        matchMetadata(metadata);
    }
}

/**
 * @brief SemanticVersion::fromString
 * @param value
 */
SemanticVersion SemanticVersion::fromString(QString str)
{
    // Setup patterns as stated in "Semantic Version 2.0.0"

    // 2. A normal version number MUST take the form X.Y.Z where X, Y, and Z are
    //    non-negative integers, and MUST NOT contain leading zeroes. [...].
    const QString versionPattern = "(" + VERSION_PATTERN + ")";
    const int VERSION_INDEX = 1;

    // 9. A pre-release version MAY be denoted by appending a hyphen and a series
    //    of dot separated identifiers immediately following the patch version.
    //    Identifiers MUST comprise only ASCII alphanumerics and hyphen
    //    [0-9A-Za-z-].  Identifiers MUST NOT be empty.  Numeric identifiers MUST
    //    NOT include leading zeroes.
    const QString preReleasePattern = "(-" + PRERELEASE_PATTERN + ")?";
    const int PRERELEASE_INDEX = 6;

    // 10. Build metadata MAY be denoted by appending a plus sign and a series of
    //     dot separated identifiers immediately following the patch or
    //     pre-release version.  Identifiers MUST comprise only ASCII
    //     alphanumerics and hyphen [0-9A-Za-z-].  Identifiers MUST NOT be
    //     empty.
    const QString metadataPattern = "(\\+" + METADATA_PATTERN + ")?";
    const int METADATA_INDEX = 9;

    const QString regexPattern = QString("^") + versionPattern + preReleasePattern + metadataPattern + "$";

    QRegularExpression regex(regexPattern);
    QRegularExpressionMatch match = regex.match(str);

    if (!match.hasMatch())
    {
        qDebug() << "Invalide version:" << str << "Regex Pattern = " << regexPattern;
        throw;
    }

    // ... else start parsing
    return SemanticVersion(parseSemanticVersion(match.captured(VERSION_INDEX)), match.captured(PRERELEASE_INDEX), match.captured(METADATA_INDEX));
}

/**
 * @brief parseSemanticVersion
 * @param str
 * @return
 */
QVector<int> SemanticVersion::parseSemanticVersion(const QString &str)
{
    QVector<int> results;

    QRegularExpression regex("\\d+");
    QRegularExpressionMatchIterator i = regex.globalMatch(str);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        results.push_back(match.captured().toInt());
    }


    return results;
}

/**
 * @brief SemanticVersion::parsePreRelease
 */
void SemanticVersion::parsePreRelease()
{
    QRegularExpression regex("\\.");
    QRegularExpressionMatchIterator i = regex.globalMatch(preRelease);

    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        QString str = match.captured();
        checkIdentifier(str);
        preReleaseComp.push_back(PreReleaseComponent(str));
    }
}

/**
 * @brief SemanticVersion::checkIdentifier
 * @param str
 */
void SemanticVersion::checkIdentifier(const QString &str)
{
    if (str.isEmpty())
    {
        qDebug() << "Invalid identifier: " << str;
        throw;
    }

    if (str[0] != '0') return;

    for (int i = 1; i < str.size(); ++i)
    {
        if (!str.at(i).isDigit()) return;
    }

    qDebug() << "Numerical identifier cannot contain leading zeroes: " << str;
    throw;
}

/**
 * @brief SemanticVersion::matchPreRelease
 * @param str
 */
void SemanticVersion::matchPreRelease(const QString &str)
{
    QRegularExpression regex(PRERELEASE_PATTERN);
    QRegularExpressionMatch match = regex.match(str);

    if (!match.hasMatch())
    {
        qDebug() << "Invalid prerelease: " << str;
        throw;
    }
}

/**
 * @brief SemanticVersion::matchMetadata
 * @param str
 */
void SemanticVersion::matchMetadata(const QString &str)
{
    QRegularExpression regex(METADATA_PATTERN);
    QRegularExpressionMatch match = regex.match(str);

    if (!match.hasMatch())
    {
        qDebug() << "Invalid metadata: " << str;
        throw;
    }
}

/**
 * @brief SemanticVersion::getVersion
 * @param index
 * @return
 */
int SemanticVersion::getSemanticVersion(int index) const
{
    if (index >= versions.size()) return 0;

    return versions[index];
}

/**
 * @brief SemanticVersion::getPreRelease
 * @return
 */
QString SemanticVersion::getPreRelease() const
{
    return preRelease;
}

/**
 * @brief SemanticVersion::getMetadata
 * @return
 */
QString SemanticVersion::getMetadata() const
{
    return metadata;
}

/**
 * @brief SemanticVersion::operator >
 * @param value
 * @return
 */
bool SemanticVersion::operator>(const SemanticVersion &value) const
{
    return (value < *this);
}

/**
 * @brief SemanticVersion::operator ==
 * @param value
 * @return
 */
bool SemanticVersion::operator==(const SemanticVersion &value) const
{
    return versions == value.versions && preRelease == value.preRelease;
}

/**
 * @brief SemanticVersion::operator <
 * @param value
 * @return
 */
bool SemanticVersion::operator<(const SemanticVersion &value) const
{
    // Compare version numbers.
    if (versions < value.versions) return true;
    if (versions > value.versions) return false;

    // Compare prerelease identifiers.
    if (preRelease == value.preRelease) return false;

    // If either one, but not both, are release versions, release is greater.
    if (isRelease() ^ value.isRelease()) return !isRelease();

    return preReleaseComp < value.preReleaseComp;
}

/**
 * @brief SemanticVersion::isRelease
 * @return
 */
bool SemanticVersion::isRelease() const
{
    return (preRelease.isEmpty());
}

/**
 * @brief SemanticVersion::stripPreRelease
 * @return
 */
SemanticVersion SemanticVersion::stripPreRelease() const
{
    return SemanticVersion(versions, "", metadata);
}

/**
 * @brief SemanticVersion::stripMetadata
 * @return
 */
SemanticVersion SemanticVersion::stripMetadata() const
{
    return SemanticVersion(versions, preRelease, "");
}

/**
 * @brief SemanticVersion::str
 * @return
 */
QString SemanticVersion::str() const
{
    QString out = QString::number(versions.at(0));

    for (int i = 1; i < versions.size(); ++i)
    {
        out += ".";
        out += QString::number(versions.at(i));
    }

    if (!preRelease.isEmpty())
    {
        out += "-";
        out += preRelease;
    }

    if (!metadata.isEmpty())
    {
        out += "+";
        out += metadata;
    }

    return out;
}
