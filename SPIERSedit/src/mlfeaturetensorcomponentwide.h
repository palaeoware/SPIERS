/**
 * @file
 * Header: Mlfeaturetensorcomponentwide
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
#ifndef MLFEATURETENSORCOMPONENTWIDE_H
#define MLFEATURETENSORCOMPONENTWIDE_H

#include <QObject>
#include "mlfeature.h"

class MLFeatureTensorComponentWide : public MLFeature
{
public:
    enum class TensorComponent
    {
        XX = 0,
        YY,
        ZZ,
        XY,
        XZ,
        YZ
    };

    MLFeatureTensorComponentWide(Channel channel, int arg1, TensorComponent component);
    MLFeatureTensorComponentWide(Channel channel, int arg1, int arg2);

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
    int GetMinMaxForArgs(int arg, bool max) override;

protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;

private:
    TensorComponent GetComponent() const;
    QString GetComponentString() const;
    int GetIntegrationRadiusLog2() const;

public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;
};

#endif // MLFEATURETENSORCOMPONENTWIDE_H
