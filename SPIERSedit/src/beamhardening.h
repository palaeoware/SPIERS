#ifndef BEAMHARDENING_H
#define BEAMHARDENING_H

#include <QImage>
#include <QList>
#include <QListWidget>
#include <QLabel>

class BeamHardening
{
public:
    BeamHardening();
    void Measure(QListWidget *SliceSelectorList, int cX, int cY, QLabel *label, int radius);
    int GetCorrectionAtWorkingImageCoordinates(int x, int y);
    bool HasSample();
    void SetParams(int x, int y, int radius, int adjust);
private:
    int *sampleBuffer, *sampleCountBuffer;
    int sampleBufferSize;
    long Sample(int file, int centerX, int centerY);
    bool IsLocked(int x, int y);
    int cx, cy, rad, adj;
};

#endif // BEAMHARDENING_H
