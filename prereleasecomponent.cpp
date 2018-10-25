/**
 * @file
 * Pre-release Component
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

#include "prereleasecomponent.h"

/**
 * @brief PreReleaseComponent::PreReleaseComponent
 * @param str
 */
PreReleaseComponent::PreReleaseComponent(QString str)
{
    identifier = str;

    isNumber = !identifier.isEmpty() && identifier.end()->isNumber();

    valueAsNumber = isNumber ? identifier.toInt() : 0;
}

/**
 * @brief PreReleaseComponent::operator ==
 * @param value
 * @return
 */
bool PreReleaseComponent::operator==(const PreReleaseComponent &value) const
{
    return identifier == value.identifier;
}

/**
 * @brief PreReleaseComponent::operator <
 * @param value
 * @return
 */
bool PreReleaseComponent::operator<(const PreReleaseComponent &value) const
{
    if (isNumber && value.isNumber) return valueAsNumber < value.valueAsNumber;
    if (!isNumber && !value.isNumber) return identifier < value.identifier;
    return isNumber;
}

/**
 * @brief PreReleaseComponent::operator >
 * @param value
 * @return
 */
bool PreReleaseComponent::operator>(const PreReleaseComponent &value) const
{
    return (value < *this);
}
