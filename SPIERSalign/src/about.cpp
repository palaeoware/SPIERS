/**
 * @file
 * About Dialog
 *
 * All SPIERSalign code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSalign code is Copyright 2008-2019 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "about.h"
#include "globals.h"
#include "mainwindowimpl.h"

#include <QString>

/**
 * @brief About::About
 * @param parent
 */
About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    setWindowTitle("About");
    setWindowIcon(QIcon (":/icon.png"));

    ui->header->setVisible(false);

    ui->textLabel_1->setText(theMainWindow->windowTitle());
    ui->textLabel_1->setObjectName("aboutTextLabel1");
    ui->textLabel_1->setAlignment(Qt::AlignCenter);

    ui->textLabel_2->setWordWrap(true);
    ui->textLabel_2->setText("This  software is " + QString(PRODUCTNAME) +
                             ". It was coded by"
                             " Russell Garwood (russell.garwood@gmail.com), with advice and assistance from Mark Sutton (m.sutton@imperial.ac.uk) and additional code by Alan R.T. Spencer (alan.spencer@imperial.ac.uk)"
                             "<br><br>Reports are appreciated, and comments, suggestions, and feature requests are welcome.");
    ui->textLabel_2->setAlignment(Qt::AlignCenter);


    ui->textLabel_3->setWordWrap(true);
    ui->textLabel_3->setText("<b>Copyright and License:</b>"
                             "<br><br>" + QString(COPYRIGHT) +
                             "<br><br>" + QString(LICENCE) + " (see below).");
    ui->textLabel_3->setAlignment(Qt::AlignCenter);

    ui->textBrowser->setHtml(returnLicense());
    ui->textBrowser->setOpenLinks(true);
    ui->textBrowser->setOpenExternalLinks(true);

    QPixmap picture_2(":/license/gplV3Logo");
    ui->footer1->setPixmap(picture_2);
    ui->footer1->setAlignment(Qt::AlignCenter);

    QPixmap picture_3(":/license/builtWithQTLogo");
    ui->footer2->setPixmap(picture_3);
    ui->footer2->setAlignment(Qt::AlignCenter);

    ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
}

/**
 * @brief About::~About
 */
About::~About()
{
    delete ui;
}

/**
 * @brief About::returnLicense
 * @return
 */
QString About::returnLicense()
{
    QFile file(":/license/license");

    if (file.open(QIODevice::ReadOnly))
    {
        QString license = file.readAll();
        file.close();
        return license;
    }
    else return "License Missing!";
}
