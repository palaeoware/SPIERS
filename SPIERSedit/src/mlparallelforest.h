/**
 * @file
 * Header: Mlparallelforest
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
#ifndef MLPARALLELFOREST_H
#define MLPARALLELFOREST_H

#include <QList>
#include <QVector>

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

class MLParallelForest
{
public:
    MLParallelForest();
    ~MLParallelForest();

    void Clear();
    bool IsValid() const;

    void SetShardCount(int shardCount);
    int GetShardCount() const;

    void SetTreeCount(int treeCount);
    int GetTreeCount() const;

    void SetMinSampleCount(int minSampleCount);
    int GetMinSampleCount() const;

    void SetMaxDepth(int maxDepth);
    int GetMaxDepth() const;

    int GetClassCount() const;
    int GetTotalTreeCount() const;

    bool Train(const cv::Mat &trainData,
               const cv::Mat &labels,
               int classCount);

    bool PredictProbabilities(const cv::Mat &samples,
                              cv::Mat &probabilities) const;

    cv::Mat GetVarImportance() const;

private:
    struct Shard
    {
        cv::Ptr<cv::ml::RTrees> model;
        int treeCount = 0;
    };

    static Shard TrainOneShard(const cv::Mat &trainData,
                               const cv::Mat &labels,
                               int treeCount,
                               int minSampleCount,
                               int maxDepth);

    static QList<int> SplitTreeCounts(int totalTreeCount, int shardCount);

    static bool ConvertRawVotesToSampleClassVotes(const cv::Mat &rawVotes,
                                                  int classCount,
                                                  cv::Mat &sampleVotes);

private:
    QVector<Shard> _shards;

    int _shardCount = 4;
    int _treeCount = 100;
    int _minSampleCount = 10;
    int _maxDepth = 10;
    int _classCount = 0;
};

#endif // MLPARALLELFOREST_H