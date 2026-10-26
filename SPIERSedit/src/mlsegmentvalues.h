/**
 * @file
 * Header: Mlsegmentvalues
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
#ifndef MLSEGMENTVALUES_H
#define MLSEGMENTVALUES_H

#include <QList>
#include <QVector>

/**
 *
 * Pure helpers for the ML system's handling of active (visible) and absent (hidden)
 * segments. They use no global state, so they can be tested in isolation.
 * See .claude/specs/ml-absent-segments.md.
 *
 **/
class MLSegmentValues
{
public:
    /**
     *
     * Converts per-segment model probabilities (0..1) into working-image values (0..255).
     * Untrained segments get 0. If no trained-and-writable segment reaches 128, the
     * highest-valued one (lowest index on ties) is raised to 128. Values are produced for
     * every segment; callers must only write those that are writable.
     *
     **/
    static void probabilitiesToSegmentValues(
        const float *probabilities,
        int segmentCount,
        const QVector<bool> &trained,
        const QVector<bool> &writable,
        int *values);

    /**
     *
     * Returns the indices of labels whose segment is active, in their original order, and
     * fills countsPerSegment (resized to active.size()) with the number of such labels per
     * segment. Labels with a segment index outside 0..active.size()-1 are ignored.
     *
     **/
    static QVector<int> selectTrainingRows(
        const QVector<int> &labelSegments,
        const QVector<bool> &active,
        QVector<int> *countsPerSegment);

    /**
     *
     * Returns the active segments whose count is below minimumSamples, in index order.
     *
     **/
    static QList<int> segmentsShortOfSamples(
        const QVector<int> &countsPerSegment,
        const QVector<bool> &active,
        int minimumSamples);
};

#endif // MLSEGMENTVALUES_H
