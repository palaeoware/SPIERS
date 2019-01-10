#include "watershed.h"
#include "globals.h"

#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

/**
 * @brief watershedSegment
 */
void watershed(QImage *imagePointer)
{
    // Only call is CurrentMode == 6
    if (CurrentMode != 6)
        return;

    // Get source slice image from ColArray
    QImage srcImage;

    // Are we dealing with a 8-bit index or RGB(A) image of some kind?
    if (GreyImage)
    {
        // Is grayscale 8-bit index so we need to convert it to a normal 24-bit rgb QImage
        QImage temp = ColArray;

        // Add grayscale colour lookup table
        QVector<QRgb> lookup;
        for (int i = 0; i < 256; i++)
            lookup.append(qRgb(i, i, i));
        temp.setColorTable(lookup);

        srcImage = temp.convertToFormat(QImage::Format_RGB888);
    }
    else
    {
        // Otherwise it is a colour image so just convert to a known format
        srcImage = ColArray.convertToFormat(QImage::Format_RGB888);
    }

    QImage watershedImage = watershedByMarker(srcImage);

    // Highlight everything that is set to black in the watershed image
    // this should represent everything that is not segmented
    // maybe issues with large areas turning red, but that is a problem for a
    // latter stage
    for (int r = 0; r < watershedImage.width(); r++)
    {
        for (int c = 0; c < watershedImage.width(); c++)
        {
            //imagePointer->setPixel(r, c, watershedImage.pixel(r, c));
            if (watershedImage.pixel(r, c) == qRgb(255, 255, 255))
                imagePointer->setPixel(r, c, qRgb(255, 0, 0));
        }
    }
}

/**
 * @brief watershedByMarker
 * @param srcImage
 * @param markerImage
 * @return
 */
QImage watershedByMarker(QImage srcImage)
{
    QImage watershedImage;

    // Convert QImage to OpenCV Mat
    QImage temp = srcImage.rgbSwapped();
    Mat src = Mat(temp.height(), temp.width(), CV_8UC3, const_cast<uchar *>(temp.bits()), static_cast<size_t>(temp.bytesPerLine())).clone();

    // To store grayscale version of the src
    Mat gray;

    // To store thresholded version of src
    Mat thresh;

    // Convert scr to grayscale
    cvtColor(src, gray, CV_BGR2GRAY);

    // Threshold image to binary form with Otsu
    threshold(gray, thresh, 40, 255, CV_THRESH_BINARY_INV + CV_THRESH_OTSU);

    // Perform morphological open on thresholded image
    morphologyEx(thresh, thresh, MORPH_OPEN, Mat::ones(9, 9, CV_8SC1), Point(4, 4), 2);

    // Distance transform
    Mat distance(thresh.rows, thresh.cols, CV_32FC1);
    distanceTransform(thresh, distance, CV_DIST_L2, 3);

    // Normalise data
    normalize(distance, distance, 0.0, 1, NORM_MINMAX);
    //imshow("Distance Transformation", distance);

    // Threshold the distance image to obtain markers for watershed
    threshold(distance, distance, 0.3, 1, CV_THRESH_BINARY);

    // Renormalize to 0-255 to further calculations
    normalize(distance, distance, 0.0, 255.0, NORM_MINMAX);
    distance.convertTo(distance, CV_8UC1);
    //imshow("Thresholded Distance Transformation",distance);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(distance, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

    if (contours.empty())
    {
        qDebug() << "Contours empty";
        return srcImage;
    }

    Mat markers(distance.size(), CV_32S);
    markers = Scalar::all(0);

    // Draw contours
    int i, j, compCount = 0;
    int idx = 0;
    for (; idx >= 0; idx = hierarchy[idx][0], compCount++ )
        drawContours(markers, contours, idx, Scalar::all(compCount + 1), -1, 8, hierarchy, INT_MAX);

    // Watershed
    watershed(src, markers);

    // Render segments
    vector<Vec3b> colorTab;
    for ( i = 0; i < compCount; i++ )
    {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);
        colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }


    Mat result(markers.size(), CV_8UC3);
    for ( i = 0; i < markers.rows; i++ )
        for ( j = 0; j < markers.cols; j++ )
        {
            int index = markers.at<int>(i, j);
            if ( index == -1 )
                result.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
            else if ( index <= 0 || index > compCount )
                result.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            else
                result.at<Vec3b>(i, j) = colorTab[index - 1];
        }

    // Convert OpenCV Mat to QImage
    watershedImage = QImage(result.data, result.cols, result.rows, static_cast<int>(result.step), QImage::Format_RGB888).rgbSwapped();

    return watershedImage;
}

/**
 * @brief watershedByGradient
 * @param srcImage
 * @param gradientImage
 * @return
 */
QImage watershedByGradient(QImage srcImage)
{
    QImage watershedImage;
    watershedImage = srcImage;

    return watershedImage;
}


