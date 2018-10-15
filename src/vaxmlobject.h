#ifndef VAXMLOBJECT_H
#define VAXMLOBJECT_H

#include <QString>

/**
 * @brief The VAXMLObject class
 */
class VAXMLObject
{
public:
    bool visible;
    float matrix[16];
    float transparency;
    int red;
    int green;
    int blue;
    int position;
    QChar key;
    QString name;
    QString file;
    QString url;
    QString ingroup;

    VAXMLObject();
};

#endif // VAXMLOBJECT_H
