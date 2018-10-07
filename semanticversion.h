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
