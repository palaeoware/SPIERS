/**
 * @file
 * Header: Find Poly Nomial Impl.
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef FINDPOLYNOMIALIMPL_H
#define FINDPOLYNOMIALIMPL_H

#include <QCloseEvent>
#include <QDialog>

#include "ui_findpolynomial.h"

/**
 * @brief The SampleEntry class
 */
class SampleEntry
{
public:
    SampleEntry(uchar r, uchar g, uchar b, int s);
    uchar r, g, b;
    int seg;
};

/**
 * @brief The findpolynomialImpl class
 */
class findpolynomialImpl : public QDialog, public Ui::findpolynomial
{
    Q_OBJECT
public:
    findpolynomialImpl( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
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
