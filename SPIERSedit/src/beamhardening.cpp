#include "globals.h"
#include "beamhardening.h"
#include "fileio.h"
#include <QList>
#include <QtMath>

BeamHardening::BeamHardening()
{
    sampleBufferSize = -1;
}

void BeamHardening::SetParams(int x, int y, int radius, int adjust)
{
    cx = x;
    cy = y;
    rad = radius;
    adj = adjust;
}

void BeamHardening::Measure(QListWidget *SliceSelectorList, int cX, int cY, QLabel *label, int radius)
{
    WriteAllData(CurrentFile);

    long pixelCount = 0;
    int fileCount = 0;

    for (int i = 0; i < Files.count(); i++)
    {
        if ((SliceSelectorList->item(i))->isSelected()) fileCount++;
    }
    if (sampleBufferSize!=-1)
    {
        free(sampleBuffer); //kill old ones - unless first time
        free(sampleCountBuffer);
    }

    if (fileCount == 0)
    {
        label->setText("No files with locked (selected) pixels are selected. No measurements made.");
        return;
    }

    sampleBufferSize = qMax(cwidth, cheight); //really only needs root2 x max, but this will be safer
    sampleBuffer = (int *)malloc(sampleBufferSize*sizeof(int));
    sampleCountBuffer = (int *)malloc(sampleBufferSize*sizeof(int));

    for (int i=0; i<sampleBufferSize; i++)
    {
        sampleBuffer[i]=0;
        sampleCountBuffer[i]=0;
    }

    int thisFcount=0;
    for (int i = 0; i < Files.count(); i++)
    {
        //for each file
        if ((SliceSelectorList->item(i))->isSelected())
        {
            label->setText(QString("Measuring file %1 of %2").arg(++thisFcount).arg(fileCount));
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            pixelCount += Sample(i, cX, cY);
        }
    }

    if (pixelCount == 0)
    {
        label->setText("No files with locked (selected) pixels are selected. No measurements made.");
        free(sampleBuffer); //kill old ones - unless first time
        free(sampleCountBuffer);
        sampleBufferSize=-1;
        return;
    }

    for (int i=0; i<sampleBufferSize; i++)
    {
        if (sampleCountBuffer[i]!=0)
        {
            sampleBuffer[i]/=sampleCountBuffer[i];
        }
    }

    label->setText(QString("Measurement completed. %1 pixels sampled over %2 slices").arg(pixelCount).arg(fileCount));

    //work out mean value in middle
    long total = 0;
    long totalDivisor = 0;
    for (int i=1; i<=radius; i++) //ignore 1
    {
        total += sampleBuffer[i] * sampleCountBuffer[i]; //for better mean
        totalDivisor += sampleCountBuffer[i];
    }

    int targetLevel = (int)(total/totalDivisor);

    //and adjust the values throughout - yes, even inside in case radius is changed after sampling
    for (int i=0; i<sampleBufferSize; i++)
    {
        if (sampleCountBuffer[i]>0)
            sampleBuffer[i] = targetLevel - sampleBuffer[i];
        else
            sampleBuffer[i] = 0;
        qDebug()<<sampleBuffer[i] << sampleCountBuffer[i];
    }

    //Done - sample buffer is now how much we should increase by at that distance

    //restore setup
    LoadAllData(CurrentFile);
}

bool BeamHardening::HasSample()
{
    if (sampleBufferSize==-1) return false; else return true;
}

int BeamHardening::GetCorrectionAtWorkingImageCoordinates(int x, int y)
{
    x *= ColMonoScale;
    y *= ColMonoScale;

    int dx = cx - x;
    int dy = cy - y;
    int dist = (int)qSqrt(dx*dx + dy*dy);

    if (dist<rad)
        return adj;

    if (sampleCountBuffer[dist]==0)
        return adj;

    return sampleBuffer[dist] + adj;
}


long BeamHardening::Sample(int slice, int centerX, int centerY)
{
    long count=0;
    LoadColourData(slice);
    LoadLocks(slice);

    for (int x=0; x<cwidth; x++)
        for (int y=0; y<cheight; y++)
        {
            if (IsLocked(x,y))
            {
                //work out distance
                int dx = x-centerX;
                int dx2 = dx * dx;
                int dy = y-centerY;
                int dy2 = dy * dy;
                qreal dist = qSqrt(dx2 + dy2);
                int iDist = (int)dist;

                sampleBuffer[iDist] += ColArray.pixelColor(x,y).red();
                sampleCountBuffer[iDist]++;

                count++;
            }
        }
    return count;
}

bool BeamHardening::IsLocked(int x, int y)
{
    int xWk = x / ColMonoScale;
    int yWk = y / ColMonoScale;
    uchar *data = (uchar *) Locks.data();

    int pos = ((fheight - yWk - 1) * fwidth + xWk) * 2;
    return (data[pos] == 255);
}





///Stuff not in use
void BeamHardening::FindCentre(QList<int> *slices,int maxRadius)
{

    if (slices->count()==0) return;

    if (maxRadius<0) maxRadius = 5;

    //make array to accumulate average values
    int *accumulator = (int *)malloc(ColArray.width()*ColArray.height()*sizeof(int));

    for (int i=0; i<cwidth; i++)
    for (int j=0; j<cheight; j++)
        accumulator[i + j * cwidth] = 0;

    for (int i = 0; i < slices->count(); i++)
    {
        LoadColourData(i);
            CombineIntoData((*slices)[i], accumulator);
    }


    //no need to calculate a mean. Just use the full data.


    //Center is pixel with most even falloff in all directions.
    //To score a pixel - we make a falloff array of some length in a bunch of directions - going both ways, and looking at difference between both

    int *buffer = (int *)malloc(maxRadius*sizeof(int));
    double bestScore= 1000000.0f;
    int bestX, bestY;

    for (int currentGuessX = ((double)cwidth * .4); currentGuessX < ((double)cwidth * .6); currentGuessX++)
    {
        qDebug()<<"X="<<currentGuessX;
            for (int currentGuessY = ((double)cheight * .4); currentGuessY < ((double)cheight * .6); currentGuessY++)
        {
            double currentScore = ScorePixel(currentGuessX, currentGuessY, maxRadius, accumulator, buffer);
            if (currentScore<bestScore)
            {
                bestScore = currentScore;
                bestX = currentGuessX;
                bestY = currentGuessY;
                qDebug()<<"New best "<<bestX<<bestY<<bestScore;
            }
            qApp->processEvents();

        }
    }

    qDebug()<<"Final result: best center is "<<bestScore<<bestX<<bestY;

    free((void *)accumulator);
    free((void *)buffer);
}

void BeamHardening::CombineIntoData(int slice, int *accumulator)
{
    LoadColourData(slice);
    for (int i=0; i<ColArray.width(); i++)
    for (int j=0; j<ColArray.width(); j++)
    {
        QColor colour = ColArray.pixelColor(i,j);
        accumulator[i + j * cwidth] += colour.red(); //just use red - assume greyscale!
    }
}

double BeamHardening::ScorePixel(int x, int y, int bufferSize, int *accumulator, int *buffer)
{
    int count = 0;
    int accumDifference = 0;
    int maxCount = 0;

    for (int i=0; i<bufferSize; i++) buffer[i]=0;

    for (int i=0; i<10; i++)
    {
        count =0;
        double angle = i * (2 * M_PI) / 100.0;
        double xAdd = qSin(angle);
        double yAdd = qCos(angle);

        for (int offset = 1; offset<bufferSize; offset++)
        {
            double xPos = (double)x + xAdd * (double)offset;
            double yPos = (double)y + yAdd * (double)offset;

            if (xPos <0 || xPos >=cwidth-.501 || yPos < 0 || yPos >= cheight-.501)
                break;

            int left = SampleAccumulator(xPos, yPos, accumulator);


            //and the other way
            xPos = (double)x - xAdd * (double)offset;
            yPos = (double)y - yAdd * (double)offset;

            if (xPos <0 || xPos >=cwidth-.501 || yPos < 0 || yPos >= cheight-.501)
                break;

            int right = SampleAccumulator(xPos, yPos, accumulator);

            buffer[offset] = qAbs(left-right);
            count++;
        }

        if (count>maxCount) maxCount = count;
    }

    int finalTotal = 0;
    for (int i=1; i<maxCount; i++)
        finalTotal += buffer[i];

    //qDebug()<<"Score for pixel "<<x<<","<<y<<" is "<<(double)finalTotal/(double)maxCount;

    return (double)finalTotal/(double)maxCount;
}

int BeamHardening::SampleAccumulator(double x, double y, int *accumulator)
{
    //we use nearest neighbour value
    int xIndex = (int)(x+.5);
    int yIndex = (int)(y+.5);

    return accumulator[xIndex + yIndex * cwidth];

}

/*
uchar BeamHardening::GreyScalePixel(int w, int h, int r, int g, int b, int glob)
{
    w *= ColMonoScale;
    h *= ColMonoScale;
    int rtot = 0;
    uchar *data = ColArray.bits();
    int temp;
    if (GreyImage)
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                rtot += static_cast<int>(data[n + m * cwidth4]) * (glob - 1) / 50;
            }
        temp = rtot / (ColMonoScale * ColMonoScale);
    }
    else
    {
        for (int n = w; n < (w + ColMonoScale); n++)
            for (int m = h; m < (h + ColMonoScale); m++)
            {
                int p = 4 * (n + m * cwidth);
                rtot += static_cast<int>(RED(data, p)) * r;
                rtot += static_cast<int>(GREEN(data, p)) * g;
                rtot += static_cast<int>(BLUE(data, p)) * b;
            }
        rtot *= glob;
        temp = rtot / (ColMonoScale * ColMonoScale * 10000);
    }


    if (temp < 0) temp= 0;
    if (temp > 255) temp= 255;
    return static_cast<uchar>(temp);
}
*/
