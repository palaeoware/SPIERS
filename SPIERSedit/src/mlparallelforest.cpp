#include "mlparallelforest.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QDebug>
#include "globals.h"

MLParallelForest::MLParallelForest()
{
}

MLParallelForest::~MLParallelForest()
{
    Clear();
}

void MLParallelForest::Clear()
{
    _shards.clear();
    _classCount = 0;
}

bool MLParallelForest::IsValid() const
{
    if (_shards.isEmpty())
        return false;

    for (const Shard &shard : _shards)
    {
        if (shard.model.empty())
            return false;
    }

    return true;
}

void MLParallelForest::SetShardCount(int shardCount)
{
    if (shardCount < 1)
        shardCount = 1;

    _shardCount = shardCount;

    qDebug()<<"Shard count now "<<shardCount;
}

int MLParallelForest::GetShardCount() const
{
    return _shardCount;
}

void MLParallelForest::SetTreeCount(int treeCount)
{
    if (treeCount < 1)
        treeCount = 1;

    _treeCount = treeCount;
}

int MLParallelForest::GetTreeCount() const
{
    return _treeCount;
}

void MLParallelForest::SetMinSampleCount(int minSampleCount)
{
    if (minSampleCount < 1)
        minSampleCount = 1;

    _minSampleCount = minSampleCount;
}

int MLParallelForest::GetMinSampleCount() const
{
    return _minSampleCount;
}

void MLParallelForest::SetMaxDepth(int maxDepth)
{
    if (maxDepth < 1)
        maxDepth = 1;

    _maxDepth = maxDepth;
}

int MLParallelForest::GetMaxDepth() const
{
    return _maxDepth;
}

int MLParallelForest::GetClassCount() const
{
    return _classCount;
}

int MLParallelForest::GetTotalTreeCount() const
{
    int total = 0;
    for (const Shard &shard : _shards)
        total += shard.treeCount;
    return total;
}

QList<int> MLParallelForest::SplitTreeCounts(int totalTreeCount, int shardCount)
{
    QList<int> counts;

    if (shardCount < 1)
        shardCount = 1;
    if (totalTreeCount < 1)
        totalTreeCount = 1;

    counts.reserve(shardCount);

    const int base = totalTreeCount / shardCount;
    const int remainder = totalTreeCount % shardCount;

    for (int i = 0; i < shardCount; ++i)
    {
        int count = base;
        if (i < remainder)
            ++count;

        if (count < 1)
            count = 1;

        counts.append(count);
    }

    return counts;
}

MLParallelForest::Shard MLParallelForest::TrainOneShard(const cv::Mat &trainData,
                                                        const cv::Mat &labels,
                                                        int treeCount,
                                                        int minSampleCount,
                                                        int maxDepth)
{
    Shard shard;
    shard.treeCount = treeCount;
    shard.model = cv::ml::RTrees::create();

    shard.model->setCalculateVarImportance(true);
    shard.model->setMaxDepth(maxDepth);
    shard.model->setMinSampleCount(minSampleCount);
    shard.model->setRegressionAccuracy(0.0f);
    shard.model->setUseSurrogates(false);
    shard.model->setMaxCategories(SegmentCount);
    shard.model->setTermCriteria(cv::TermCriteria(cv::TermCriteria::MAX_ITER, treeCount, 0));

    cv::Ptr<cv::ml::TrainData> td = cv::ml::TrainData::create(
        trainData,
        cv::ml::ROW_SAMPLE,
        labels
        );

    shard.model->train(td);

    return shard;
}

bool MLParallelForest::Train(const cv::Mat &trainData,
                             const cv::Mat &labels,
                             int classCount)
{
    Clear();

    if (trainData.empty())
        return false;
    if (labels.empty())
        return false;
    if (trainData.rows != labels.rows)
        return false;
    if (labels.cols != 1)
        return false;
    if (classCount < 1)
        return false;

    QList<int> treeCounts = SplitTreeCounts(_treeCount, _shardCount);

    QVector<QFuture<Shard>> futures;
    futures.reserve(treeCounts.count());

    for (int i = 0; i < treeCounts.count(); ++i)
    {
        const int shardTreeCount = treeCounts[i];

        futures.append(QtConcurrent::run(
            [trainData, labels, shardTreeCount, this]() -> Shard
            {
                return TrainOneShard(
                    trainData,
                    labels,
                    shardTreeCount,
                    _minSampleCount,
                    _maxDepth
                    );
            }));
    }

    _shards.reserve(treeCounts.count());

    for (int i = 0; i < futures.count(); ++i)
    {
        futures[i].waitForFinished();
        Shard shard = futures[i].result();

        if (shard.model.empty())
        {
            Clear();
            return false;
        }

        _shards.append(shard);
    }

    _classCount = classCount;
    return true;
}

bool MLParallelForest::ConvertRawVotesToSampleClassVotes(const cv::Mat &rawVotes,
                                                         int classCount,
                                                         cv::Mat &sampleVotes)
{
    if (rawVotes.empty())
        return false;
    if (rawVotes.rows < 2)
        return false;
    if (classCount < 1)
        return false;
    if (rawVotes.type() != CV_32S)
        return false;

    const int voteCols = rawVotes.cols;
    const int sampleCount = rawVotes.rows - 1;

    sampleVotes = cv::Mat::zeros(sampleCount, classCount, CV_32S);

    const int *labelRow = rawVotes.ptr<int>(0);

    for (int sample = 0; sample < sampleCount; ++sample)
    {
        const int *srcRow = rawVotes.ptr<int>(sample + 1);
        int *dstRow = sampleVotes.ptr<int>(sample);

        for (int col = 0; col < voteCols; ++col)
        {
            const int label = labelRow[col];

            if (label >= 0 && label < classCount)
                dstRow[label] += srcRow[col];
        }
    }

    return true;
}

bool MLParallelForest::PredictProbabilities(const cv::Mat &samples,
                                            cv::Mat &probabilities) const
{
    probabilities.release();

    if (!IsValid())
        return false;
    if (samples.empty())
        return false;
    if (_classCount < 1)
        return false;

    QVector<QFuture<cv::Mat>> futures;
    futures.reserve(_shards.count());

    for (int i = 0; i < _shards.count(); ++i)
    {
        const Shard shard = _shards[i];

        futures.append(QtConcurrent::run(
            [shard, samples]() -> cv::Mat
            {
                cv::Mat rawVotes;
                shard.model->getVotes(samples, rawVotes, 0);
                return rawVotes;
            }));
    }

    cv::Mat totalVotes = cv::Mat::zeros(samples.rows, _classCount, CV_32S);

    for (int i = 0; i < futures.count(); ++i)
    {
        futures[i].waitForFinished();
        cv::Mat rawVotes = futures[i].result();

        cv::Mat shardVotes;
        if (!ConvertRawVotesToSampleClassVotes(rawVotes, _classCount, shardVotes))
        {
            probabilities.release();
            return false;
        }

        totalVotes += shardVotes;
    }

    const int totalTreeCount = GetTotalTreeCount();
    if (totalTreeCount < 1)
    {
        probabilities.release();
        return false;
    }

    probabilities.create(samples.rows, _classCount, CV_32F);

    const float denom = static_cast<float>(totalTreeCount);

    for (int row = 0; row < samples.rows; ++row)
    {
        const int *srcRow = totalVotes.ptr<int>(row);
        float *dstRow = probabilities.ptr<float>(row);

        for (int c = 0; c < _classCount; ++c)
            dstRow[c] = static_cast<float>(srcRow[c]) / denom;
    }

    return true;
}

cv::Mat MLParallelForest::GetVarImportance() const
{
    if (!IsValid())
        return cv::Mat();

    cv::Mat accum;
    int totalTrees = 0;

    for (const Shard &shard : _shards)
    {
        if (shard.model.empty())
            return cv::Mat();

        cv::Mat imp = shard.model->getVarImportance();
        if (imp.empty())
            return cv::Mat();

        cv::Mat imp32f;
        if (imp.type() != CV_32F)
            imp.convertTo(imp32f, CV_32F);
        else
            imp32f = imp;

        if (accum.empty())
        {
            accum = cv::Mat::zeros(imp32f.rows, imp32f.cols, CV_32F);
        }
        else
        {
            if (accum.rows != imp32f.rows || accum.cols != imp32f.cols)
                return cv::Mat();
        }

        accum += imp32f * static_cast<float>(shard.treeCount);
        totalTrees += shard.treeCount;
    }

    if (totalTrees < 1)
        return cv::Mat();

    accum /= static_cast<float>(totalTrees);
    return accum;
}