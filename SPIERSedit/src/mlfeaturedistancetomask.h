/**
 * @file
 * Header: Mlfeaturedistancetomask
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
#ifndef MLFEATUREDISTANCETOMASK_H
#define MLFEATUREDISTANCETOMASK_H

#include "mlfeature.h"

class MLFeatureDistanceToMask : public MLFeature
{
public:
    explicit MLFeatureDistanceToMask(int maskId);

    void CalculateFeature(cv::Mat &mat, int sliceID, MLCachedAccess *data) override;
    QList<MLFeature *> GetDependencies() override;
    QString GetPrettyName() override;
    QString GetPrettyArgs() override;
    QString GetPretty3D() override;
    int GetDependencyDepth() override;
    QString GetArg1SetupString(int value) override;
    QString GetArg2SetupString(int value) override;
    int GetMinMaxForArgs(int arg, bool max) override;
    bool ReferencesMask(int maskId) const override;
    void RemapMasks(const QVector<int> &maskMap) override;

protected:
    QString GetTypeCodeForFile() override;
    QString GetArgsForFile() override;
    QString Get3DForFile() override;

private:
    void calculateVolume();
};

#endif // MLFEATUREDISTANCETOMASK_H
