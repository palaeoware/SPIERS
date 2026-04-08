/**
 * @file
 * Header: Mlfeaturepresets
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
#ifndef MLFEATUREPRESETS_H
#define MLFEATUREPRESETS_H

#include <QList>
#include "mlfeature.h"

class MLFeaturePresets
{
public:
    enum class Preset
    {
        CT_Simple,
        CT_Complex,
        Colour_Simple,
        Colour_Complex
    };

    static QList<MLFeature*> GetPresetFeatureList(Preset preset);

private:
    static QList<MLFeature*> CT_Simple();
    static QList<MLFeature*> CT_Complex();
    static QList<MLFeature*> Colour_Simple();
    static QList<MLFeature*> Colour_Complex();
};

#endif