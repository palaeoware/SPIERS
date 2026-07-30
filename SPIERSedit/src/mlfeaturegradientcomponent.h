/**
 * @file
 * Header: Mlfeaturegradientcomponent
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
#ifndef MLFEATUREGRADIENTCOMPONENT_H
#define MLFEATUREGRADIENTCOMPONENT_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureGradientComponent : public MLFeature
{
public:
    enum class GradientDirection
    {
        X = 0,
        Y,
        Z
    };

    MLFeatureGradientComponent(Channel channel, int arg1, GradientDirection dir);
    MLFeatureGradientComponent(Channel channel, int arg1, int arg2);

public:
    void CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data) override;
    bool CalculateFeatureROI(
        cv::Mat &mat,
        int sliceID,
        MLCachedAccess *data,
        const MLROISlice &roi) override;
    QList<MLFeature *> GetDependencies() override;
    QString GetPrettyName() override;
    QString GetPrettyArgs() override;
    QString GetPretty3D() override;

    int GetDependencyDepth() override;

protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;

private:
    GradientDirection GetDirection() const;
    QString GetDirectionString() const;

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;

    // MLFeature interface
public:
    int GetMinMaxForArgs(int arg, bool max) override;
};

#endif // MLFEATUREGRADIENTCOMPONENT_H
