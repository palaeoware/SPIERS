/**
 * @file
 * Source: Mlsegmentvalues
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
#include "mlsegmentvalues.h"

void MLSegmentValues::probabilitiesToSegmentValues(
    const float *probabilities,
    int segmentCount,
    const QVector<bool> &trained,
    const QVector<bool> &writable,
    int *values)
{
    int highValue = -1;
    int highSegment = -1;

    for (int s = 0; s < segmentCount; s++)
    {
        const bool isTrained = s < trained.size() && trained.at(s);
        if (!isTrained)
        {
            values[s] = 0;
            continue;
        }

        int value = static_cast<int>(probabilities[s] * 255.0f + 0.5f);
        if (value < 0) value = 0;
        if (value > 255) value = 255;
        values[s] = value;

        const bool isWritable = s < writable.size() && writable.at(s);
        if (isWritable && value > highValue)
        {
            highValue = value;
            highSegment = s;
        }
    }

    //ensure no black - the best segment we can actually write claims the pixel
    if (highSegment != -1 && highValue < 128)
        values[highSegment] = 128;
}

QVector<int> MLSegmentValues::selectTrainingRows(
    const QVector<int> &labelSegments,
    const QVector<bool> &active,
    QVector<int> *countsPerSegment)
{
    QVector<int> rows;
    rows.reserve(labelSegments.size());

    if (countsPerSegment != nullptr)
        countsPerSegment->fill(0, active.size());

    for (int i = 0; i < labelSegments.size(); i++)
    {
        const int segment = labelSegments.at(i);
        if (segment < 0 || segment >= active.size() || !active.at(segment))
            continue;

        rows.append(i);
        if (countsPerSegment != nullptr)
            (*countsPerSegment)[segment]++;
    }

    return rows;
}

QList<int> MLSegmentValues::segmentsShortOfSamples(
    const QVector<int> &countsPerSegment,
    const QVector<bool> &active,
    int minimumSamples)
{
    QList<int> shortSegments;
    for (int s = 0; s < active.size(); s++)
    {
        if (!active.at(s))
            continue;

        const int count = s < countsPerSegment.size() ? countsPerSegment.at(s) : 0;
        if (count < minimumSamples)
            shortSegments.append(s);
    }
    return shortSegments;
}
