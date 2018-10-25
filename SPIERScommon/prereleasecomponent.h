/**
 * @file
 * Header: Pre-release Component
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
