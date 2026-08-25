/**
 * @file
 * Header: Mlfeatureintensity
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
#ifndef MLFEATUREINTENSITY_H
#define MLFEATUREINTENSITY_H

#include "mlfeature.h"

class MLFeatureIntensity : public MLFeature
{
public:
    MLFeatureIntensity(Channel channel);
    void CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data) override;
    bool CalculateFeatureROI(
        cv::Mat &mat,
        int sliceID,
        MLCachedAccess *data,
        const MLROISlice &roi) override;
private:
    void CalcFeatureIntensity(cv::Mat &mat, int sliceID, MLCachedAccess *data);
    void CalcFeatureColor(cv::Mat &mat, int sliceID, MLCachedAccess *data);

    // MLFeature interface
public:
    QString GetPrettyName() override;
    QString GetPrettyArgs() override;
    QString GetPretty3D() override;

protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;
    // MLFeature interface
public:
    QList<MLFeature *> GetDependencies() override;

    // MLFeature interface
public:
    int GetDependencyDepth() override;

    // MLFeature interface
public:
    QString GetArg1SetupString(int v) override;
    QString GetArg2SetupString(int v) override;

    // MLFeature interface
public:
    int GetMinMaxForArgs(int arg, bool max) override;
};

#endif // MLFEATUREINTENSITY_H
