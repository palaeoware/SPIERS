#ifndef PRERELEASECOMPONENT_H
#define PRERELEASECOMPONENT_H

#include <QString>

class PreReleaseComponent
{
public:
    PreReleaseComponent(QString str = "");
    bool operator<(const PreReleaseComponent &value) const;
    bool operator>(const PreReleaseComponent &value) const;
    bool operator==(const PreReleaseComponent &value) const;

private:
    bool isNumber;
    QString identifier;
    int valueAsNumber;

};

#endif // PRERELEASECOMPONENT_H
