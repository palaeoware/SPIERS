#ifndef COPYINGIMPL_H
#define COPYINGIMPL_H
//
#include "ui_Copying.h"
#include "mainwindowimpl.h"
#include <QDir>
#include <QList>
#include <QListWidget>

//
class OutputObject;
class CopyingImpl : public QDialog, public Ui::Copying
{
    Q_OBJECT
public:
    CopyingImpl( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    void Copy(QDir source, QDir dest);
    void MakeNewSegFiles(int snum);
    void DeleteSegments(QList <int> list);
    void GenerateLinear(QListWidget *SliceSelectorList);
    void GeneratePoly(QListWidget *SliceSelectorList);
    void GenerateRange(QListWidget *SliceSelectorList);
    void GenerateAllLinear();
    void GenerateAllBlank();
    void ExportSPV(int);
    void ExportSPV_2(int flag);
    void MeasureVols();
    void MaskCopy(int fromfile, MainWindowImpl *mw);
    void MaskCopy2(int fromfile, MainWindowImpl *mw);
    void ReverseStretches(QList <double> *stretches, int Sstart, int Sstop);
    void DoOutputRecursive(QList <int> *translationtable, int *nexttransentry, QList <int> *translationtable2, int *nexttransentry2, int parent, long long int asize, int filesused, int awidth,
                           int aheight, QDataStream *out, QList <double> *stretches);
    void GetOutputList(QList <OutputObject *> *outlist, QList <int> *translationtable, int *nexttransentry, QList <int> *translationtable2, int *nexttransentry2, int parent);
    void CurvesToMasks(MainWindowImpl *mw);
    void CompressAllWorkingFiles(int level);
    void CompressAllSourceFiles(int level);
    void Apply3DBrush(int button);
    bool copying;
protected:
    void closeEvent(QCloseEvent *event);
private:
    bool AmIMerged(int n);
    void StupidBodgyFunction(QList <int> *tt, int parent);
    bool DoIHaveChildren(int parent);
    void MakeMergeObject(int *, QByteArray *, QVector <double> *TrigArray, int *TrigCount, int parent, long long int asize, int filesused, int awidth, int aheight, QDataStream *out,
                         QList <double> *stretches);
    void MakeMergeObject_2(int, OutputObject *);
    void WriteSPVData(int, QByteArray, QVector<double> *TrigArray, int TrigCount, QDataStream *out);
    QByteArray ExpandGrid(QByteArray *grid, int awidth, int aheight);
    QString CountMessage;
    int Count;
private slots:
    void escape();

};
#endif





