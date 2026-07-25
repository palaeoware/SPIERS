#include "mlsegmenttomask.h"
#include "globals.h"
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <climits>
#include <cmath>
#include <vector>

mlSegmentToMask::mlSegmentToMask()
{

}

void mlSegmentToMask::execute(int segID, int maskID, const QVector<int> &segMap, int directionPairCount, int scoreThreshold)
{
    if (segID < 0 || segID >= SegmentCount)
    {
        qWarning() << "Invalid segment ID passed to mlSegmentToMask::execute:" << segID;
        return;
    }

    if (maskID < 0 || maskID > UCHAR_MAX)
    {
        qWarning() << "Invalid mask ID passed to mlSegmentToMask::execute:" << maskID;
        return;
    }

    if (directionPairCount <= 0)
    {
        qWarning() << "Invalid direction-pair count passed to mlSegmentToMask::execute:" << directionPairCount;
        return;
    }

    if (scoreThreshold < 0 || scoreThreshold > directionPairCount * 2)
    {
        qWarning() << "Invalid score threshold passed to mlSegmentToMask::execute:" << scoreThreshold;
        return;
    }

    const int maskSize = fwidth * fheight;
    if (Masks.size() != maskSize)
    {
        qWarning() << "Mask size mismatch in mlSegmentToMask::execute:" << Masks.size() << "expected" << maskSize;
        return;
    }

    if (segMap.size() != maskSize)
    {
        qWarning() << "Segment map size mismatch in mlSegmentToMask::execute:" << segMap.size() << "expected" << maskSize;
        return;
    }

    cv::Mat thresholded(fheight, fwidth, CV_8UC1, cv::Scalar(0));

    for (int y = 0; y < fheight; y++)
    {
        uchar *thresholdedRow = thresholded.ptr<uchar>(y);

        for (int x = 0; x < fwidth; x++)
        {
            if (segMap[y * fwidth + x] == segID)
            {
                thresholdedRow[x] = 255;
            }
        }
    }

    cv::Mat score(thresholded.rows, thresholded.cols, CV_16UC1, cv::Scalar(0));

    const auto incrementLine = [&thresholded, &score](const std::vector<cv::Point> &linePixels)
    {
        const int lineLength = static_cast<int>(linePixels.size());
        int firstRunStart = -1;
        int firstRunEnd = -1;
        int lastRunStart = -1;
        int runCount = 0;
        int runStart = -1;

        for (int index = 0; index < lineLength; index++)
        {
            const int x = linePixels[index].x;
            const int y = linePixels[index].y;

            if (thresholded.at<uchar>(y, x) != 0)
            {
                if (runStart == -1)
                {
                    runStart = index;
                }
            }
            else if (runStart != -1)
            {
                if (runCount == 0)
                {
                    firstRunStart = runStart;
                    firstRunEnd = index - 1;
                }

                lastRunStart = runStart;
                runCount++;
                runStart = -1;
            }
        }

        if (runStart != -1)
        {
            if (runCount == 0)
            {
                firstRunStart = runStart;
                firstRunEnd = lineLength - 1;
            }

            lastRunStart = runStart;
            runCount++;
        }

        if (runCount == 0)
        {
            return;
        }

        for (int index = 0; index < lineLength; index++)
        {
            uchar increment = 1;
            if (runCount == 1)
            {
                if (index >= firstRunStart && index <= firstRunEnd)
                {
                    increment = 2;
                }
            }
            else if (index >= firstRunEnd && index <= lastRunStart)
            {
                increment = 2;
            }

            const int x = linePixels[index].x;
            const int y = linePixels[index].y;
            score.at<quint16>(y, x) = static_cast<quint16>(score.at<quint16>(y, x) + increment);
        }
    };

    constexpr double pi = 3.14159265358979323846;
    for (int directionIndex = 0; directionIndex < directionPairCount; directionIndex++)
    {
        const double angle = (pi * static_cast<double>(directionIndex)) / static_cast<double>(directionPairCount);
        const double directionX = std::cos(angle);
        const double directionY = std::sin(angle);
        const double normalX = -directionY;
        const double normalY = directionX;

        const int cornerKeys[4] =
        {
            static_cast<int>(std::lround(0.0)),
            static_cast<int>(std::lround((fwidth - 1) * normalX)),
            static_cast<int>(std::lround((fheight - 1) * normalY)),
            static_cast<int>(std::lround((fwidth - 1) * normalX + (fheight - 1) * normalY))
        };

        const int minKey = *std::min_element(cornerKeys, cornerKeys + 4);
        const int maxKey = *std::max_element(cornerKeys, cornerKeys + 4);
        std::vector<std::vector<cv::Point>> lines(static_cast<size_t>(maxKey - minKey + 1));

        for (int y = 0; y < thresholded.rows; y++)
        {
            for (int x = 0; x < thresholded.cols; x++)
            {
                const int key = static_cast<int>(std::lround(x * normalX + y * normalY)) - minKey;
                lines[static_cast<size_t>(key)].push_back(cv::Point(x, y));
            }
        }

        for (std::vector<cv::Point> &linePixels : lines)
        {
            std::sort(linePixels.begin(),
                      linePixels.end(),
                      [directionX, directionY](const cv::Point &a, const cv::Point &b)
                      {
                          return a.x * directionX + a.y * directionY < b.x * directionX + b.y * directionY;
                      });
            incrementLine(linePixels);
        }
    }

    const uchar maskValue = static_cast<uchar>(maskID);
    for (int i = 0; i < Masks.size(); i++)
    {
        if (static_cast<uchar>(Masks[i]) == maskValue)
        {
            Masks[i] = 0;
        }
    }

    for (int y = 0; y < score.rows; y++)
    {
        const quint16 *scoreRow = score.ptr<quint16>(y);
        const uchar *thresholdedRow = thresholded.ptr<uchar>(y);
        const int maskY = score.rows - y - 1;

        for (int x = 0; x < score.cols; x++)
        {
            if (scoreRow[x] >= scoreThreshold || thresholdedRow[x] != 0)
            {
                Masks[maskY * score.cols + x] = maskValue;
            }
        }
    }
}
