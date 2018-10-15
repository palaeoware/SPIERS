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
