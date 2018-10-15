#ifndef FINDPOLYNOMIALIMPL_H
#define FINDPOLYNOMIALIMPL_H
//
#include <QCloseEvent>

#include <QDialog>
#include "ui_findpolynomial.h"
//

class SampleEntry 
{
	public:
	SampleEntry(uchar r, uchar g,uchar b, int s);
	uchar r, g, b; int seg;
};

class findpolynomialImpl : public QDialog, public Ui::findpolynomial
{
Q_OBJECT
public:
    findpolynomialImpl( QWidget * parent = 0, Qt::WindowFlags f = 0 );
        void find();
protected:
 	void closeEvent(QCloseEvent *event);
private slots:
	void on_pushButton_pressed();
private:
	QList <SampleEntry> Sample;
	bool pleasestop;
	bool stopped;
	void Breed();
	double polyrand();
	void RandomizePoly(int s, int o);
	int MeasurePolyFitness();
	double PolyTweak(double val);
	int  BreedPoly(int gens);
	double CalcScale();
	void UnStashPoly();
	void StashPoly();


};
#endif





