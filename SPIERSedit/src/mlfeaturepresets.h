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