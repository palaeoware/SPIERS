#ifndef VAXMLGROUP_H
#define VAXMLGROUP_H

#include <QString>

/**
 * @brief The VAXMLGroup class
 */
class VAXMLGroup
{
public:
    bool visible;
    int position;
    QChar key;
    QString name;
    QString ingroup;

    VAXMLGroup();
};

#endif // VAXMLGROUP_H
