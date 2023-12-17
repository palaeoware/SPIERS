#include "globals.h"
#include "beamhardening.h"
#include "fileio.h"
#include <QList>
#include <QtMath>

BeamHardening::BeamHardening()
{
    sampleBufferSize = -1;  //this is an internal code for 'no data'
}

void BeamHardening::SetParams(int x, int y, int radius, int adjust)
{
    cx = x;
    cy = y;
    rad = radius;
    adj = adjust;
}

//Get the sample, and compute the correction buffer
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
        free(sampleBuffer); //free the memory
        free(sampleCountBuffer);
        sampleBufferSize=-1; //and reset to 'no data'
        return;
    }

    //divide totals by counts
    for (int i=0; i<sampleBufferSize; i++)
    {
        if (sampleCountBuffer[i]!=0)
        {
            sampleBuffer[i]/=sampleCountBuffer[i];
        }
    }

    label->setText(QString("Measurement completed. %1 pixels sampled over %2 slices").arg(pixelCount).arg(fileCount));

    //work out mean value in middle region
    long total = 0;
    long totalDivisor = 0;
    for (int i=1; i<=radius; i++) //ignore 1
    {
        total += sampleBuffer[i] * sampleCountBuffer[i]; //for better mean
        totalDivisor += sampleCountBuffer[i];
    }
    if (totalDivisor == 0)
    {
        label->setText("No data selected. No measurements made.");
        free(sampleBuffer); //free the memory
        free(sampleCountBuffer);
        sampleBufferSize=-1; //and reset to 'no data'
        return;
    }

    int targetLevel = (int)(total/totalDivisor);    //no, this can't be a divide by zero

    //and adjust the values throughout - yes, even inside radius in case radius is changed after sampling
    for (int i=0; i<sampleBufferSize; i++)
    {
        if (sampleCountBuffer[i]>0)
            sampleBuffer[i] = targetLevel - sampleBuffer[i];  //compute correction
        else
            sampleBuffer[i] = 0;
    }

    //Done - sample buffer is now how much we should increase by at that distance

    //restore setup
    LoadAllData(CurrentFile);

}

bool BeamHardening::HasSample()
{
    if (sampleBufferSize==-1)
        return false;
    else
        return true;
}

int BeamHardening::GetCorrectionAtWorkingImageCoordinates(int x, int y)
{
    x *= ColMonoScale;
    y *= ColMonoScale;

    int dx = cx - x;
    int dy = cy - y;
    int dist = (int)qSqrt(dx*dx + dy*dy);

    if (dist<rad)   //inside
        return adj;

    if (sampleCountBuffer[dist]==0) //no data
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

                sampleBuffer[iDist] += ColArray.pixelColor(x,y).red(); //use red value as proxy for greyscale value
                sampleCountBuffer[iDist]++;

                count++;
            }
        }
    return count;
}

//check for pixel lock
bool BeamHardening::IsLocked(int x, int y)
{
    int xWk = x / ColMonoScale;
    int yWk = y / ColMonoScale;
    uchar *data = (uchar *) Locks.data();

    int pos = ((fheight - yWk - 1) * fwidth + xWk) * 2; //magic formula copied from elsewhere. Why *2? Who knows!
    return (data[pos] == 255);
}

