/**
 * @file
 * Source: About
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2019 by Alan R.T. Spencer, Russell J. Garwood,
 * and Mark D. Sutton.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "globals.h"
#include "mainwindow.h"

/**
 * @brief aboutdialog::aboutdialog
 * @param parent
 */
aboutdialog::aboutdialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::aboutdialog)
{
    m_ui->setupUi(this);

    setWindowTitle("About");
    setWindowIcon(QIcon(":/ViewIcon.bmp"));

    m_ui->header->setVisible(false);

    m_ui->textLabel_1->setText(QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION));
    m_ui->textLabel_1->setObjectName("aboutTextLabel1");
    m_ui->textLabel_1->setAlignment(Qt::AlignCenter);

    m_ui->textLabel_2->setWordWrap(true);
    m_ui->textLabel_2->setText("This  software is " + QString(PRODUCTNAME) + " v" + QString(SOFTWARE_VERSION) +
                               ". It was coded by Alan R.T. Spencer (alan.spencer@imperial.ac.uk)."
                               "<br><br>Reports are appreciated, and comments, suggestions, and feature requests are welcome.");
    m_ui->textLabel_2->setAlignment(Qt::AlignCenter);

    m_ui->textLabel_3->setWordWrap(true);
    m_ui->textLabel_3->setText("<b>Copyright and License:</b>"
                               "<br><br>" + QString(COPYRIGHT) +
                               "<br><br>" + QString(LICENCE) + " (see below).");
    m_ui->textLabel_3->setAlignment(Qt::AlignCenter);

    m_ui->textBrowser->setHtml(returnLicense());
    m_ui->textBrowser->setOpenLinks(true);
    m_ui->textBrowser->setOpenExternalLinks(true);

    QPixmap picture_2(":/license/gplV3Logo");
    m_ui->footer1->setPixmap(picture_2);
    m_ui->footer1->setAlignment(Qt::AlignCenter);

    QPixmap picture_3(":/license/builtWithQTLogo");
    m_ui->footer2->setPixmap(picture_3);
    m_ui->footer2->setAlignment(Qt::AlignCenter);

    m_ui->buttonBox->setStandardButtons(QDialogButtonBox::Close);
}

/**
 * @brief aboutdialog::~aboutdialog
 */
aboutdialog::~aboutdialog()
{
    delete m_ui;
}

/**
 * @brief aboutdialog::changeEvent
 * @param e
 */
void aboutdialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/**
 * @brief aboutdialog::on_closebutton_clicked
 */
void aboutdialog::on_closebutton_clicked()
{
    close();
}

/**
 * @brief aboutdialog::returnLicense
 * @return
 */
QString aboutdialog::returnLicense()
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

