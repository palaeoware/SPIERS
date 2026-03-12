#include "mlfeaturecalculator.h"
#include "mlupdateblockingdialog.h"

#include "globals.h"
MLFeatureCalculator::MLFeatureCalculator()
{


}


/* Feature naming system
 *
 * src: src image, at binned res
 *
 * Lets use 3/4 letter codes. If 4, last letter is 2 or 3 for 2D/3D. If necessary we have args
 * Separators are @
 *
 * DONE
 * gau = gaussian. Arg is PoT of sigma. So gau@1, gau@4, gau@0, gau@-1
 * dog = diference of gaussian. Two args, the gau levels. So dog@4@2. Highest first
 * int = intensity. arg is t,r,g,b (t is total, i.e. grey) NOT YET DONE COL ARGS
 * mea = local mean - arg is radius
 * cnt = contrast (=intensity - gaussian). PoT sigma again
 *
 * TODO
 * grd = gradient. x,y,z so grd@x etc
 * lap = laplacian - not sure of args
 * log = laplacian of gaussian - arg is sigma PoT again
 * var = local var
 * hes = hessian - two args
 * ten = structure tensor - two args

 *
*/

QList<QString> MLFeatureCalculator::GetDependencies(QString featureName)
{
    QList<QString> dependencyList;
    QStringList elements = featureName.toLower().split('@');

    if (elements[0]=="mea3")
        dependencyList.append(QString("mea2@%1").arg(elements[1]));

    if (elements[0]=="gau3")
        dependencyList.append(QString("gau2@%1").arg(elements[1]));

    if (elements[0]=="dog3")
    {
        dependencyList.append(QString("gau3@%1").arg(elements[1]));
        dependencyList.append(QString("gau3@%1").arg(elements[2]));
    }

    if (elements[0]=="dog2")
    {
        dependencyList.append(QString("gau2@%1").arg(elements[1]));
        dependencyList.append(QString("gau2@%1").arg(elements[2]));
    }

    if (elements[0]=="cnt2")
        dependencyList.append(QString("gau2@%1").arg(elements[1]));

    if (elements[0]=="cnt3")
        dependencyList.append(QString("gau3@%1").arg(elements[1]));

    return dependencyList;
}


void MLFeatureCalculator::CalculateFeature(cv::Mat &mat, int sliceID, QString featureName, MLCachedAccess *data)
{
    //Switchboard method
    QStringList elements = featureName.toLower().split('@');

    if (elements[0] == "int")
    {
        if (elements.count()==1) CalcFeatureIntensity(mat, sliceID, data);
        else
        {
            if (elements[1] == "t") CalcFeatureIntensity(mat, sliceID, data);
            else CalcFeatureColor(mat, sliceID, data, elements[1]);
        }
    }

    else if (elements[0] == "mea3")
        CalcFeatureMean3D(mat, sliceID, data, elements[1]);

    else if (elements[0] == "mea2")
        CalcFeatureMean2D(mat, sliceID, data, elements[1]);

    else if (elements[0] == "gau2")
        CalcFeatureGaussian2D(mat, sliceID, data, elements[1]);

    else if (elements[0] == "gau3")
        CalcFeatureGaussian3D(mat, sliceID, data, elements[1]);

    else if (elements[0] == "dog3")
    {
        CalcFeatureDifferenceOfFeatures(mat, sliceID, data,
                                        QString("gau3@%1").arg(elements[1]),
                                        QString("gau3@%1").arg(elements[2])
                                        );
    }

    else if (elements[0] == "dog2")
    {
        CalcFeatureDifferenceOfFeatures(mat, sliceID, data,
                                        QString("gau2@%1").arg(elements[1]),
                                        QString("gau2@%1").arg(elements[2])
                                        );
    }

    else if (elements[0] == "cnt3")
    {
        CalcFeatureDifferenceOfFeatures(mat, sliceID, data,
                                        QString("src"),
                                        QString("gau3@%1").arg(elements[1])
                                        );
    }

    else if (elements[0] == "cnt2")
    {
        CalcFeatureDifferenceOfFeatures(mat, sliceID, data,
                                        QString("src"),
                                        QString("gau2@%1").arg(elements[1])
                                        );
    }


}

void MLFeatureCalculator::CalcFeatureIntensity(cv::Mat &mat, int sliceID, MLCachedAccess *data)
{
    int xSize = data->GetXSize();
    int ySize = data->GetYSize();

    for (int y=0; y<ySize; y++)
    {
        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
            QString("Calculating intensity feature for slice %1: %2%").arg(sliceID)
                    .arg((y*100)/ySize)
            );
        for (int x=0; x<xSize; x++)
        {
                mat.at<float>(y,x) = data->GetIntensityAsFloat(x,y,sliceID);
        }
    }
}

void MLFeatureCalculator::CalcFeatureColor(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString col)
{
    int xSize = data->GetXSize();
    int ySize = data->GetYSize();

    if (col=="r")
    {
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating red intensity feature for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                mat.at<float>(y,x) = data->GetRGBFloat(x,y,sliceID).redF();
            }
        }
    }
    else if (col=="g")
    {
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating green intensity feature for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                mat.at<float>(y,x) = data->GetRGBFloat(x,y,sliceID).greenF();
            }
        }
    }
    else if (col=="b")
    {
        for (int y=0; y<ySize; y++)
        {
            if (y%50==0)
                MLUpdateBlockingDialog::updateDetailText(
                    QString("Calculating blue intensity feature for slice %1: %2%")
                        .arg(sliceID)
                        .arg((y*100)/ySize)
                    );
            for (int x=0; x<xSize; x++)
            {
                mat.at<float>(y,x) = data->GetRGBFloat(x,y,sliceID).blueF();
            }
        }
    }
}

void MLFeatureCalculator::CalcFeatureMean3D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString radius)
{
    bool ok;

    int r = radius.toInt(&ok);
    if (!ok)
    {
        qDebug()<<"illegal argument in feature Mean3D: "<<radius;
        return;
    }

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    //Get featureID for the Mean2D feature
    int mea2index = data->GetIndexForFeature(QString("mea2@") + radius);

    //Get all the slices I need
    int z0 = std::max(0, sliceID - r);
    int z1 = std::min(FileCount - 1, sliceID + r);

    QVector<cv::Mat> slices;
    for (int i=z0; i<=z1; i++)
    {
        slices.append(data->GetWholeSliceFeature(i, mea2index));
    }


    for (int y = 0; y < fheight; ++y)
    {

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating 3D mean feature at radius %1 for slice %2: %3%")
                    .arg(radius)
                    .arg(sliceID)
                    .arg((y*100)/fheight)
                );
        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double sum = 0.0;
            int count = 0;

            for (int z = z0; z <= z1; ++z)
            {
                sum += slices[z-z0].at<float>(y,x);
                ++count;
            }

            outRow[x] = (count > 0) ? static_cast<float>(sum / count) : 0.0f;
        }
    }
}


void MLFeatureCalculator::CalcFeatureMean2D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString radius)
{
    bool ok;

    int r = radius.toInt(&ok);
    if (!ok)
    {
        qDebug()<<"illegal argument in feature Mean3D: "<<radius;
        return;
    }

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    cv::Mat slice = data->GetWholeSliceIntensity(sliceID);

    for (int y = 0; y < fheight; ++y)
    {

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating 2D mean feature at radius %1 for slice %2: %3%")
                    .arg(radius)
                    .arg(sliceID)
                    .arg((y*100)/fheight)
                );
        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double sum = 0.0;
            int count = 0;

            int y0 = std::max(0, y - r);
            int y1 = std::min(fheight - 1, y + r);

            int x0 = std::max(0, x - r);
            int x1 = std::min(fwidth - 1, x + r);


            for (int yy = y0; yy <= y1; ++yy)
            {
                float *row = slice.ptr<float>(yy);
                for (int xx = x0; xx <= x1; ++xx)
                {
                    sum += row[xx];
                    ++count;
                }
            }


            outRow[x] = (count > 0) ? static_cast<float>(sum / count) : 0.0f;
        }
    }
}

void MLFeatureCalculator::CalcFeatureGaussian2D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString sigmaStr)
{
    bool ok;

    float sigma = sigmaStr.toFloat(&ok);
    if (!ok)
    {
        qDebug()<<"illegal argument in feature Gaussian2D: "<<sigmaStr;
        return;
    }

    sigma = std::pow(2,sigma);

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    cv::Mat slice = data->GetWholeSliceIntensity(sliceID);

    int r = static_cast<int>(ceil(3.0 * sigma));
    int ksize = 2 * r + 1;

    std::vector<float> kernel(ksize);

    double sum = 0.0;
    for (int k = -r; k <= r; ++k)
    {
        double v = exp(-(k*k) / (2.0 * sigma * sigma));
        kernel[k + r] = static_cast<float>(v);
        sum += v;
    }

    for (float &v : kernel)
        v /= sum;

    cv::Mat temp(fheight, fwidth, CV_32F);

    // --- horizontal pass ---

    for (int y = 0; y < fheight; ++y)
    {
        float *inRow = slice.ptr<float>(y);
        float *outRow = temp.ptr<float>(y);

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating 2D gaussian feature sigma %1 for slice %2: %3%")
                    .arg(sigmaStr)
                    .arg(sliceID)
                    .arg((y*50)/fheight)
                );

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int xx = std::max(0, std::min(fwidth - 1, x + k));
                acc += kernel[k + r] * inRow[xx];
            }

            outRow[x] = static_cast<float>(acc);
        }
    }

    // --- vertical pass ---

    for (int y = 0; y < fheight; ++y)
    {

        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating 2D gaussian feature sigma %1 for slice %2: %3%")
                    .arg(sigmaStr)
                    .arg(sliceID)
                    .arg(50+(y*50)/fheight)
                );

        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int k = -r; k <= r; ++k)
            {
                int yy = std::max(0, std::min(fheight - 1, y + k));
                float *row = temp.ptr<float>(yy);

                acc += kernel[k + r] * row[x];
            }

            outRow[x] = static_cast<float>(acc);
        }
    }
}

void MLFeatureCalculator::CalcFeatureDifferenceOfFeatures(cv::Mat &mat, int sliceID,
                                                          MLCachedAccess *data, QString feature1, QString feature2)
{

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    MLUpdateBlockingDialog::updateDetailText(
        QString("Calculating difference (%1-%2) for slice %3%")
            .arg(feature1)
            .arg(feature2)
            .arg(sliceID)
        );

    cv::Mat data1, data2;

    if (feature1=="src")
    {
        data1 = data->GetWholeSliceIntensity(sliceID);
    }
    else
    {
        int feat1Index = data->GetIndexForFeature(feature1);
        data1 = data->GetWholeSliceFeature(sliceID, feat1Index);
    }

    if (feature2=="src")
    {
        data2 = data->GetWholeSliceIntensity(sliceID);
    }
    else
    {
        int feat2Index = data->GetIndexForFeature(feature2);
        data2 = data->GetWholeSliceFeature(sliceID, feat2Index);
    }

    cv::subtract(data1, data2, mat);
}

void MLFeatureCalculator::CalcFeatureGaussian3D(cv::Mat &mat, int sliceID, MLCachedAccess *data, QString sigmaStr)
{
    bool ok;

    float sigma = sigmaStr.toFloat(&ok);
    if (!ok)
    {
        qDebug()<<"illegal argument in feature Gaussian3D: "<<sigmaStr;
        return;
    }

    sigma = std::pow(2,sigma);

    Q_ASSERT(mat.type() == CV_32F);
    Q_ASSERT(mat.cols == fwidth);
    Q_ASSERT(mat.rows == fheight);
    Q_ASSERT(sliceID >= 0 && sliceID < FileCount);

    int gau2index = data->GetIndexForFeature(QString("gau2@") + sigmaStr);

    int r = static_cast<int>(ceil(3.0 * sigma));
    int ksize = 2 * r + 1;

    std::vector<float> kernel(ksize);

    double sum = 0.0;
    for (int k = -r; k <= r; ++k)
    {
        double v = exp(-(k * k) / (2.0 * sigma * sigma));
        kernel[k + r] = static_cast<float>(v);
        sum += v;
    }

    for (float &v : kernel)
        v /= sum;

    int z0 = std::max(0, sliceID - r);
    int z1 = std::min(FileCount - 1, sliceID + r);

    QVector<cv::Mat> slices;
    QVector<float> zweights;

    for (int i = z0; i <= z1; i++)
    {
        slices.append(data->GetWholeSliceFeature(i, gau2index));

        int dz = i - sliceID;
        zweights.append(kernel[dz + r]);
    }

    double wsum = 0.0;
    for (float w : zweights)
        wsum += w;

    for (int y = 0; y < fheight; ++y)
    {
        if (y%50==0)
            MLUpdateBlockingDialog::updateDetailText(
                QString("Calculating 3D gaussian feature log2(sigma)=%1 for slice %2: %3%")
                    .arg(sigmaStr)
                    .arg(sliceID)
                    .arg((y*100)/fheight)
                );

        float* outRow = mat.ptr<float>(y);

        for (int x = 0; x < fwidth; ++x)
        {
            double acc = 0.0;

            for (int z = z0; z <= z1; ++z)
            {
                float w = zweights[z - z0];
                acc += w * slices[z - z0].at<float>(y, x);
            }

            outRow[x] = (wsum > 0.0) ? static_cast<float>(acc / wsum) : 0.0f;
        }
    }
}

