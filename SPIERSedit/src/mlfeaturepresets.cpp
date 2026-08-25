/**
 * @file
 * Source: Mlfeaturepresets
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
#include "mlfeaturepresets.h"

#include "mlfeatureintensity.h"
#include "mlfeaturegaussian.h"
#include "mlfeaturedifferenceofgaussians.h"
#include "mlfeaturecontrast.h"
#include "mlfeaturegradient.h"
#include "mlfeaturevariance.h"
#include "mlfeaturelog.h"
#include "mlfeaturehessian.h"

#include "mlfeaturetensorcomponentlocal.h"
#include "mlfeaturetensorcomponentwide.h"
#include "mlfeaturetensortracelocal.h"
#include "mlfeaturetensortracewide.h"
#include "mlfeaturetensordeterminantlocal.h"
#include "mlfeaturetensordeterminantwide.h"
#include "mlfeaturetensorcoherencelocal.h"
#include "mlfeaturetensorcoherencewide.h"

QList<MLFeature*> MLFeaturePresets::GetPresetFeatureList(Preset preset)
{
    switch (preset)
    {
    case Preset::CT_Simple: return CT_Simple();
    case Preset::CT_Complex: return CT_Complex();
    case Preset::Colour_Simple: return Colour_Simple();
    case Preset::Colour_Complex: return Colour_Complex();
    default: return QList<MLFeature*>();
    }
}

QList<MLFeature*> MLFeaturePresets::CT_Simple()
{
    using C = MLFeature::Channel;

    QList<MLFeature*> f;

    f << new MLFeatureIntensity(C::Intensity);

    for (int s : {0,1,2,3})
        f << new MLFeatureGaussian(C::Intensity, false, s);

    for (int s : {1,2,3})
        f << new MLFeatureGaussian(C::Intensity, true, s);

    f << new MLFeatureDifferenceOfGaussians(C::Intensity, false, 0,1);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, false, 1,2);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, true, 0,1);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, true, 1,2);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, true, 2,3);

    for (int s : {1,2,3})
    {
        f << new MLFeatureContrast(C::Intensity, true, s);
        f << new MLFeatureGradient(C::Intensity, true, s);
        f << new MLFeatureVariance(C::Intensity, true, s);
    }

    return f;
}

QList<MLFeature*> MLFeaturePresets::CT_Complex()
{
    using C = MLFeature::Channel;

    QList<MLFeature*> f;

    f << new MLFeatureIntensity(C::Intensity);

    for (int s : {0,1,2,3})
    {
        f << new MLFeatureGaussian(C::Intensity, false, s);
        f << new MLFeatureGaussian(C::Intensity, true, s);
    }

    for (int s : {0,1,2,3})
    {
        f << new MLFeatureDifferenceOfGaussians(C::Intensity, false, s, s+1);
        f << new MLFeatureDifferenceOfGaussians(C::Intensity, true, s, s+1);
    }

    for (int s : {1,2,3})
    {
        f << new MLFeatureContrast(C::Intensity, true, s);
        f << new MLFeatureGradient(C::Intensity, true, s);
        f << new MLFeatureVariance(C::Intensity, true, s);
    }

    for (int s : {1,2,3})
    {
        f << new MLFeatureLoG(C::Intensity, false, s);
        f << new MLFeatureLoG(C::Intensity, true, s);
    }

    // Hessian (minimal set)
    for (int s : {2,3})
    {
        f << new MLFeatureHessian(C::Intensity, true, s, 0); // XX
        f << new MLFeatureHessian(C::Intensity, true, s, 1); // YY
        f << new MLFeatureHessian(C::Intensity, true, s, 2); // XY
        f << new MLFeatureHessian(C::Intensity, true, s, 6); // det
    }

    // Tensor scalars
    for (int s : {2,3})
    {
        f << new MLFeatureTensorTraceLocal(C::Intensity, true, s);
        f << new MLFeatureTensorDeterminantLocal(C::Intensity, true, s);
        f << new MLFeatureTensorCoherenceLocal(C::Intensity, s);
    }
    return f;
}

QList<MLFeature*> MLFeaturePresets::Colour_Simple()
{
    using C = MLFeature::Channel;

    QList<MLFeature*> f;

    QList<C> chans = {C::Intensity, C::Red, C::Green, C::Blue};

    for (auto ch : chans)
    {
        f << new MLFeatureGaussian(ch, false, 1);
        f << new MLFeatureGaussian(ch, false, 2);
        f << new MLFeatureGradient(ch, false, 1);
        f << new MLFeatureContrast(ch, false, 1);
        f << new MLFeatureVariance(ch, false, 2);
        f << new MLFeatureVariance(ch, false, 3);
    }

    for (auto ch : {C::R_G, C::G_B})
    {
        f << new MLFeatureIntensity(ch);
        f << new MLFeatureGaussian(ch, false, 1);
        f << new MLFeatureGradient(ch, false, 1);
        f << new MLFeatureContrast(ch, false, 1);
        f << new MLFeatureVariance(ch, false, 2);
        f << new MLFeatureVariance(ch, false, 3);
    }

    f << new MLFeatureGaussian(C::Intensity, true, 1);
    f << new MLFeatureGaussian(C::Intensity, true, 2);
    f << new MLFeatureGaussian(C::Intensity, true, 3);
    f << new MLFeatureGradient(C::Intensity, true, 1);

    return f;
}

QList<MLFeature*> MLFeaturePresets::Colour_Complex()
{
    using C = MLFeature::Channel;

    QList<MLFeature*> f;

    // intensity full set
    for (int s : {0,1,2,3})
    {
        f << new MLFeatureGaussian(C::Intensity, false, s);
        f << new MLFeatureGaussian(C::Intensity, true, s);
    }

    for (int s : {1,2,3})
    {
        f << new MLFeatureGaussian(C::Intensity, true, s);
        f << new MLFeatureGradient(C::Intensity, false, s);
        f << new MLFeatureGradient(C::Intensity, true, s);
        f << new MLFeatureContrast(C::Intensity, false, s);
        f << new MLFeatureVariance(C::Intensity, false, s);
    }

    f << new MLFeatureLoG(C::Intensity, false, 1);
    f << new MLFeatureLoG(C::Intensity, false, 2);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, false, 3,2);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, false, 2,1);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, true, 3,2);
    f << new MLFeatureDifferenceOfGaussians(C::Intensity, true, 2,1);
    f << new MLFeatureHessian(C::Intensity, false, 1, 6);

    f << new MLFeatureTensorTraceLocal(C::Intensity, false, 1);
    f << new MLFeatureTensorTraceWide(C::Intensity, false, 1);
    f << new MLFeatureTensorCoherenceLocal(C::Intensity, 1);
    f << new MLFeatureTensorTraceLocal(C::Intensity, false, 2);
    f << new MLFeatureTensorCoherenceLocal(C::Intensity, 2);

    // RGB moderate
    for (auto ch : {C::Red, C::Green})
    {
        f << new MLFeatureGaussian(ch, false, 1);
        f << new MLFeatureGaussian(ch, false, 3);
        f << new MLFeatureGradient(ch, false, 1);
        f << new MLFeatureContrast(ch, false, 2);
        f << new MLFeatureVariance(ch, false, 1);
        f << new MLFeatureVariance(ch, false, 2);
        f << new MLFeatureLoG(ch, false, 1);
        f << new MLFeatureLoG(ch, false, 2);
        f << new MLFeatureHessian(ch, false, 1, 6);
        f << new MLFeatureTensorTraceLocal(ch, false, 2);
        f << new MLFeatureTensorCoherenceLocal(ch, 2);
    }

    // blue lighter
    f << new MLFeatureGaussian(C::Blue, false, 1);
    f << new MLFeatureGradient(C::Blue, false, 1);
    f << new MLFeatureContrast(C::Blue, false, 1);

    // difference channels
    for (auto ch : {C::R_G, C::G_B})
    {
        f << new MLFeatureIntensity(ch);
        f << new MLFeatureGaussian(ch, false, 1);
        f << new MLFeatureGradient(ch, false, 1);
        f << new MLFeatureContrast(ch, false, 1);
        f << new MLFeatureVariance(ch, false, 1);
        f << new MLFeatureTensorTraceLocal(ch, false, 1);
        f << new MLFeatureTensorCoherenceLocal(ch, 1);

        f << new MLFeatureGaussian(ch, false, 2);
        f << new MLFeatureGradient(ch, false, 2);
        f << new MLFeatureContrast(ch, false, 2);
        f << new MLFeatureVariance(ch, false, 2);
        f << new MLFeatureTensorTraceLocal(ch, false, 2);
        f << new MLFeatureTensorCoherenceLocal(ch, 2);

        f << new MLFeatureGaussian(ch, false, 3);
        f << new MLFeatureVariance(ch, false, 3);
        f << new MLFeatureTensorTraceLocal(ch, false, 3);
    }

    return f;
}
