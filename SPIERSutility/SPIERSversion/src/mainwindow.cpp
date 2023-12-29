/**
 * @file
 * Source: MainWindow
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Alan R.T. Spencer, Russell J. Garwood,
 * and Mark D. Sutton.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QString>
#include <QTimer>
#include <QBuffer>
#include <QXmlStreamReader>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "globals.h"
#include "aboutdialog.h"

/**
 * @brief MainWindow::MainWindow
 * Constructor sets up VTK widget and one-shot timer to do load. Nothing else.
 *
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    // Setup UI
    ui->setupUi(this);

    ui->treeWidget->resizeColumnToContents(0);

    // Set global pointer to this window
    mainWindow = this;

    // Call directory dialog to select folder after 2 secs
    startTimer = new QTimer(this);
    startTimer->setSingleShot(true);
    startTimer->setInterval(2000); // 2 sec after lauch to give Splash time to its thing
    QObject::connect(startTimer, SIGNAL(timeout()), this, SLOT(startTimerFired()));
    startTimer->start();
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
}

/**
 * @brief MainWindow::getFileVersions
 */
void MainWindow::getFileVersions()
{
    QString selectedDirectory = QFileDialog::getExistingDirectory(
                                    this, tr("Select Directory"),
                                    "",
                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                                );

    // Is Cancel pressed?
    if (selectedDirectory.isNull())
    {
        return;
    }

    ui->treeWidget->clear();

    directory = QDir(selectedDirectory);

    ui->directoryPath->setText(selectedDirectory);

    files = directory.entryList(QStringList() << "*.spv" << "*.sp2" << "*.vaxml", QDir::Files);

    foreach (QString filename, files)
    {
        QFileInfo fileInfo = QFileInfo(directory, filename);

        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        //Filename
        item->setText(0, fileInfo.fileName());

        // File Size
        item->setText(1, QString::number(fileInfo.size()));

        // File type
        item->setText(2, fileInfo.completeSuffix());

        // File version
        int version = 1;
        if (fileInfo.completeSuffix() == QString("vaxml"))
        {
            // Parse VAXML
            QFile *file = new QFile(filename);
            QDataStream in(file);
            in.setByteOrder(QDataStream::LittleEndian);
            in.setVersion(QDataStream::Qt_4_5);

            QString xmlstring;
            in >> xmlstring;
            QByteArray xmlbytearray;
            xmlbytearray.append(xmlstring);
            QBuffer buffer(&xmlbytearray);

            //setup XML reader
            buffer.open(QBuffer::ReadOnly);
            QXmlStreamReader xml;
            xml.setDevice(&buffer);

            bool flag;

            if (xml.readNextStartElement())
            {
                if (xml.name() == "vaxml")
                {
                    while (xml.readNextStartElement())
                    {
                        if (xml.name() == "header")
                        {
                            while (xml.readNextStartElement())
                            {
                                if (xml.name() == "version")
                                {
                                    QString text = xml.readElementText();
                                    int i = text.toInt(&flag);
                                    version = i;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // Pasrse .spv or .sp2
            double parameter1;

            QFile fileHandler(fileInfo.absoluteFilePath());
            fileHandler.open(QIODevice::ReadOnly);
            FILE *file = fdopen(fileHandler.handle(), "rb");

            // Read the first parameters in
            fread(&parameter1, 8, 1, file);
            invertEndian(reinterpret_cast<unsigned char *>(&parameter1), 8);

            // V1 SPV files lacked a versioning system...
            // Work around is for first param to be negative in
            // v2 and up. For V2 and up, next param is int version
            // followed by real p1.
            if (parameter1 < 0)
            {
                fread(&version, sizeof(int), 1, file);
                invertEndian(reinterpret_cast<unsigned char *>(&version), 4);
            }

            fclose(file);

        }

        item->setText(3, QString::number(version));

        ui->treeWidget->resizeColumnToContents(0);
    }
}

/**
 * @brief StaticFunctions::invertEndian
 * @param data
 * @param count
 */
void MainWindow::invertEndian(unsigned char *data, int count)
{
#ifdef _BIG_ENDIAN
    unsigned char newdata[8]; //max size is double
    int n;

    for (n = 0; n < count; n++) newdata[count - 1 - n] = data[n];
    for (n = 0; n < count; n++) data[n] = newdata[n];
#else
    Q_UNUSED(data)
    Q_UNUSED(count)
#endif
    return;
}

/**
 * @brief startTimerFired
 */
void MainWindow::startTimerFired()
{
    getFileVersions();
}

/**
 * @brief MainWindow::on_actionBugIssueFeatureRequest_triggered
 */
void MainWindow::on_actionBugIssueFeatureRequest_triggered()
{

}

/**
 * @brief MainWindow::on_actionSelectDirectoryButton_triggered
 */
void MainWindow::on_selectDirectoryButton_clicked()
{
    getFileVersions();
}

/**
 * @brief MainWindow::on_actionCode_on_GitHub_triggered
 */
void MainWindow::on_actionCode_on_GitHub_triggered()
{
    QDesktopServices::openUrl(QUrl(QString(GITURL) + QString(GITREPOSITORY)));
}

/**
 * @brief MainWindow::on_actionAbout_triggered
 */
void MainWindow::on_actionAbout_triggered()
{
    aboutdialog d;
    d.exec();
}

/**
 * @brief MainWindow::on_actionExit_triggered
 */
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}
