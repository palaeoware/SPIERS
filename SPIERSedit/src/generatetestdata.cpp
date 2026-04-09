/**
 * @file
 * Source: Generate Test Data Dialog
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Russell J. Garwood, Mark D. Sutton,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "generatetestdata.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QProgressDialog>
#include <QApplication>
#include <QColorDialog>
#include <QMessageBox>
#include <QtMath>
#include <QDir>
#include <QDateTime>
#include <QStandardPaths>
#include <QRandomGenerator>

GenerateTestData::GenerateTestData(QWidget *parent)
    : QWidget(parent),
      backgroundColor(Qt::black)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Dimension controls group
    QGroupBox *dimGroup = new QGroupBox(QStringLiteral("Dataset Dimensions"), this);
    QGridLayout *dimLayout = new QGridLayout(dimGroup);

    QLabel *xLabel = new QLabel(QStringLiteral("Width (X):"), dimGroup);
    dimXSpinBox = new QSpinBox(dimGroup);
    dimXSpinBox->setMinimum(64);
    dimXSpinBox->setMaximum(4096);
    dimXSpinBox->setValue(256);
    dimLayout->addWidget(xLabel, 0, 0);
    dimLayout->addWidget(dimXSpinBox, 0, 1);

    QLabel *yLabel = new QLabel(QStringLiteral("Height (Y):"), dimGroup);
    dimYSpinBox = new QSpinBox(dimGroup);
    dimYSpinBox->setMinimum(64);
    dimYSpinBox->setMaximum(4096);
    dimYSpinBox->setValue(256);
    dimLayout->addWidget(yLabel, 1, 0);
    dimLayout->addWidget(dimYSpinBox, 1, 1);

    QLabel *zLabel = new QLabel(QStringLiteral("Slices (Z):"), dimGroup);
    dimZSpinBox = new QSpinBox(dimGroup);
    dimZSpinBox->setMinimum(10);
    dimZSpinBox->setMaximum(500);
    dimZSpinBox->setValue(50);
    dimLayout->addWidget(zLabel, 2, 0);
    dimLayout->addWidget(dimZSpinBox, 2, 1);

    mainLayout->addWidget(dimGroup);

    // Format and color mode group
    QGroupBox *formatGroup = new QGroupBox(QStringLiteral("Output Format"), this);
    QGridLayout *formatLayout = new QGridLayout(formatGroup);

    QLabel *formatLabel = new QLabel(QStringLiteral("File Format:"), formatGroup);
    formatComboBox = new QComboBox(formatGroup);
    formatComboBox->addItem(QStringLiteral("JPEG (.jpg)"));
    formatComboBox->addItem(QStringLiteral("PNG (.png)"));
    formatComboBox->setCurrentIndex(1);
    formatLayout->addWidget(formatLabel, 0, 0);
    formatLayout->addWidget(formatComboBox, 0, 1);

    QLabel *modeLabel = new QLabel(QStringLiteral("Color Mode:"), formatGroup);
    grayscaleRadio = new QRadioButton(QStringLiteral("Grayscale"), formatGroup);
    rgbRadio = new QRadioButton(QStringLiteral("RGB"), formatGroup);
    rgbRadio->setChecked(true);
    QHBoxLayout *modeLayout = new QHBoxLayout();
    modeLayout->addWidget(grayscaleRadio);
    modeLayout->addWidget(rgbRadio);
    formatLayout->addWidget(modeLabel, 1, 0);
    formatLayout->addLayout(modeLayout, 1, 1);

    mainLayout->addWidget(formatGroup);

    // Objects and options group
    QGroupBox *objectsGroup = new QGroupBox(QStringLiteral("Objects"), this);
    QGridLayout *objectsLayout = new QGridLayout(objectsGroup);

    QLabel *numObjectsLabel = new QLabel(QStringLiteral("Number of Objects:"), objectsGroup);
    numObjectsSpinBox = new QSpinBox(objectsGroup);
    numObjectsSpinBox->setMinimum(1);
    numObjectsSpinBox->setMaximum(100);
    numObjectsSpinBox->setValue(10);
    objectsLayout->addWidget(numObjectsLabel, 0, 0);
    objectsLayout->addWidget(numObjectsSpinBox, 0, 1);

    allowIntersectionCheckBox = new QCheckBox(QStringLiteral("Allow Intersections"), objectsGroup);
    allowIntersectionCheckBox->setChecked(true);
    objectsLayout->addWidget(allowIntersectionCheckBox, 1, 0, 1, 2);

    mainLayout->addWidget(objectsGroup);

    // Background color group
    QGroupBox *bgColorGroup = new QGroupBox(QStringLiteral("Background Color"), this);
    QHBoxLayout *bgColorLayout = new QHBoxLayout(bgColorGroup);

    bgColorButton = new QPushButton(QStringLiteral("Select Color"), bgColorGroup);
    bgColorLabel = new QLabel(QStringLiteral("Current:"), bgColorGroup);
    bgColorDisplayLabel = new QLabel(bgColorGroup);
    bgColorDisplayLabel->setFixedSize(40, 20);
    bgColorDisplayLabel->setStyleSheet(QStringLiteral("background-color: black; border: 1px solid gray;"));

    bgColorLayout->addWidget(bgColorButton);
    bgColorLayout->addWidget(bgColorLabel);
    bgColorLayout->addWidget(bgColorDisplayLabel);
    bgColorLayout->addStretch();

    connect(bgColorButton, &QPushButton::clicked, this, &GenerateTestData::on_BgColorButton_clicked);

    mainLayout->addWidget(bgColorGroup);

    // Generate button
    generateButton = new QPushButton(QStringLiteral("Generate Dataset"), this);
    generateButton->setStyleSheet(QStringLiteral("QPushButton { padding: 8px; font-weight: bold; }"));
    connect(generateButton, &QPushButton::clicked, this, &GenerateTestData::on_GenerateButton_clicked);
    mainLayout->addWidget(generateButton);

    mainLayout->addStretch();

    setMinimumWidth(300);
}

GenerateTestData::~GenerateTestData()
{
}

void GenerateTestData::on_GenerateButton_clicked()
{
    QString outputPath = QFileDialog::getExistingDirectory(
        this,
        QStringLiteral("Select folder for test dataset"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    if (outputPath.isEmpty())
        return;

    generateDataset(outputPath);
}

void GenerateTestData::on_BgColorButton_clicked()
{
    QColor newColor = QColorDialog::getColor(
        backgroundColor,
        this,
        QStringLiteral("Select Background Color"));

    if (newColor.isValid())
    {
        backgroundColor = newColor;
        bgColorDisplayLabel->setStyleSheet(
            QStringLiteral("background-color: rgb(%1, %2, %3); border: 1px solid gray;")
            .arg(backgroundColor.red())
            .arg(backgroundColor.green())
            .arg(backgroundColor.blue()));
    }
}

void GenerateTestData::generateDataset(const QString &outputPath)
{
    int dimX = dimXSpinBox->value();
    int dimY = dimYSpinBox->value();
    int dimZ = dimZSpinBox->value();
    int numObjects = numObjectsSpinBox->value();
    bool isGrayscale = grayscaleRadio->isChecked();
    bool allowIntersections = allowIntersectionCheckBox->isChecked();
    QString extension = formatComboBox->currentIndex() == 0 ? QStringLiteral("jpg") : QStringLiteral("png");

    QProgressDialog progressDialog(
        QStringLiteral("Generating test dataset..."),
        QStringLiteral("Cancel"),
        0, dimZ, this);
    progressDialog.setWindowModality(Qt::WindowModal);

    generateButton->setEnabled(false);

    // Generate random shapes
    QList<Shape> shapes = generateRandomShapes();

    // Generate each slice
    for (int z = 0; z < dimZ; z++)
    {
        progressDialog.setValue(z);
        QApplication::processEvents();

        if (progressDialog.wasCanceled())
        {
            generateButton->setEnabled(true);
            return;
        }

        // Create image based on color mode
        QImage image;
        if (isGrayscale)
        {
            image = QImage(dimX, dimY, QImage::Format_Grayscale8);
            image.fill(backgroundColor);
        }
        else
        {
            image = QImage(dimX, dimY, QImage::Format_RGB32);
            image.fill(backgroundColor);
        }

        // Render all shapes onto this slice
        renderSlice(image, z, shapes, allowIntersections);

        // Save image
        QString filename = QStringLiteral("%1/test_%2.%3")
                               .arg(outputPath)
                               .arg(z, 4, 10, QLatin1Char('0'))
                               .arg(extension);

        if (!image.save(filename))
        {
            QMessageBox::warning(this, QStringLiteral("Error"),
                                 QStringLiteral("Failed to save image: %1").arg(filename));
            generateButton->setEnabled(true);
            return;
        }
    }

    progressDialog.setValue(dimZ);
    generateButton->setEnabled(true);

    QMessageBox::information(this, QStringLiteral("Success"),
                             QStringLiteral("Test dataset generated successfully!\nLocation: %1\nSlices: %2 (%3x%4 pixels)")
                             .arg(outputPath)
                             .arg(dimZ)
                             .arg(dimX)
                             .arg(dimY));
}

void GenerateTestData::renderSlice(QImage &image, int sliceIndex, const QList<Shape> &shapes, bool allowIntersections)
{
    int width = image.width();
    int height = image.height();
    bool isGrayscale = image.format() == QImage::Format_Grayscale8;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            // Check all shapes to find the topmost one at this pixel
            uchar bestGrayValue = 0;
            uchar bestR = 0, bestG = 0, bestB = 0;
            bool foundShape = false;

            for (const Shape &shape : shapes)
            {
                if (isPixelInShape(x, y, sliceIndex, shape))
                {
                    // Shape found at this pixel
                    if (!foundShape)
                    {
                        foundShape = true;
                        bestGrayValue = shape.grayValue;
                        bestR = shape.r;
                        bestG = shape.g;
                        bestB = shape.b;
                    }
                    // If intersections not allowed, use first match
                    if (!allowIntersections)
                        break;
                }
            }

            if (foundShape)
            {
                if (isGrayscale)
                {
                    image.setPixelColor(x, y, QColor(bestGrayValue, bestGrayValue, bestGrayValue));
                }
                else
                {
                    image.setPixelColor(x, y, QColor(bestR, bestG, bestB));
                }
            }
        }
    }
}

bool GenerateTestData::isPixelInShape(int x, int y, int z, const Shape &shape)
{
    // Transform pixel coordinates to shape-local coordinates
    double px = x - shape.centerX;
    double py = y - shape.centerY;
    double pz = z - shape.centerZ;

    // Apply inverse rotation
    applyRotation(px, py, pz, -shape.angleX, -shape.angleY, -shape.angleZ);

    // Check if point is inside shape
    switch (shape.type)
    {
    case 0: // Rectangle
    {
        double halfX = shape.sizeX / 2.0;
        double halfY = shape.sizeY / 2.0;
        double halfZ = shape.sizeZ / 2.0;
        return (qAbs(px) <= halfX && qAbs(py) <= halfY && qAbs(pz) <= halfZ);
    }
    case 1: // Circle/Sphere
    {
        double radius = shape.sizeX / 2.0;
        double distSq = px * px + py * py + pz * pz;
        return distSq <= (radius * radius);
    }
    case 2: // Triangle (2D on XY plane)
    {
        // Simple triangle check on XY plane
        double halfX = shape.sizeX / 2.0;
        double halfY = shape.sizeY / 2.0;
        if (qAbs(pz) > shape.sizeZ / 2.0)
            return false;

        // Point-in-triangle test for XY plane
        // Triangle vertices at relative positions
        double x0 = 0, y0 = halfY;
        double x1 = -halfX, y1 = -halfY;
        double x2 = halfX, y2 = -halfY;

        auto sign = [](double px1, double py1, double px2, double py2, double px3, double py3) -> double
        {
            return (px1 - px3) * (py2 - py3) - (px2 - px3) * (py1 - py3);
        };

        double d1 = sign(px, py, x0, y0, x1, y1);
        double d2 = sign(px, py, x1, y1, x2, y2);
        double d3 = sign(px, py, x2, y2, x0, y0);

        bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(hasNeg && hasPos);
    }
    }

    return false;
}

QList<GenerateTestData::Shape> GenerateTestData::generateRandomShapes()
{
    QList<Shape> shapes;
    int numObjects = numObjectsSpinBox->value();
    int dimX = dimXSpinBox->value();
    int dimY = dimYSpinBox->value();
    int dimZ = dimZSpinBox->value();
    bool isGrayscale = grayscaleRadio->isChecked();

    QList<uchar> usedGrayValues, usedReds, usedGreens, usedBlues;

    for (int i = 0; i < numObjects; i++)
    {
        Shape shape;
        shape.type = QRandomGenerator::global()->bounded(3); // 0, 1, or 2

        // Random position
        shape.centerX = QRandomGenerator::global()->bounded(dimX);
        shape.centerY = QRandomGenerator::global()->bounded(dimY);
        shape.centerZ = QRandomGenerator::global()->bounded(dimZ);

        // Random size (30-150 pixels)
        shape.sizeX = QRandomGenerator::global()->bounded(30, 150);
        shape.sizeY = QRandomGenerator::global()->bounded(30, 150);
        shape.sizeZ = QRandomGenerator::global()->bounded(30, 150);

        // Random rotation
        shape.angleX = (QRandomGenerator::global()->generateDouble() * M_PI);
        shape.angleY = (QRandomGenerator::global()->generateDouble() * M_PI);
        shape.angleZ = (QRandomGenerator::global()->generateDouble() * M_PI);

        // Assign distinct color
        if (isGrayscale)
        {
            shape.grayValue = selectDistinctGrayValue(usedGrayValues);
            usedGrayValues.append(shape.grayValue);
            shape.r = shape.grayValue;
            shape.g = shape.grayValue;
            shape.b = shape.grayValue;
        }
        else
        {
            selectDistinctRGBValue(usedReds, usedGreens, usedBlues, shape.r, shape.g, shape.b);
            usedReds.append(shape.r);
            usedGreens.append(shape.g);
            usedBlues.append(shape.b);
        }

        shapes.append(shape);
    }

    return shapes;
}

uchar GenerateTestData::selectDistinctGrayValue(const QList<uchar> &usedValues)
{
    // Avoid very dark (0-30) and very light (220-255) to maintain contrast with background
    for (int attempt = 0; attempt < 100; attempt++)
    {
        uchar value = QRandomGenerator::global()->bounded(50, 220);

        // Check if already used
        bool used = false;
        for (uchar used : usedValues)
        {
            if (qAbs(value - used) < 20) // Minimum distance of 20
            {
                used = true;
                break;
            }
        }

        if (!used)
            return value;
    }

    // Fallback: return evenly spaced values
    return 50 + (usedValues.size() * 30);
}

void GenerateTestData::selectDistinctRGBValue(const QList<uchar> &usedRed,
                                                        const QList<uchar> &usedGreen,
                                                        const QList<uchar> &usedBlue,
                                                        uchar &r, uchar &g, uchar &b)
{
    // Try to find RGB value with good contrast
    const int minDistance = 40; // Minimum Euclidean distance from other colors
    const int minFromBg = 50;   // Minimum distance from black background (0,0,0)

    for (int attempt = 0; attempt < 200; attempt++)
    {
        r = QRandomGenerator::global()->bounded(50, 255);
        g = QRandomGenerator::global()->bounded(50, 255);
        b = QRandomGenerator::global()->bounded(50, 255);

        // Distance from background (0,0,0)
        double distFromBg = qSqrt(r * r + g * g + b * b);
        if (distFromBg < minFromBg)
            continue;

        // Check distance from other colors
        bool tooClose = false;
        for (int i = 0; i < usedRed.count(); i++)
        {
            double distSq = (r - usedRed[i]) * (r - usedRed[i]) +
                            (g - usedGreen[i]) * (g - usedGreen[i]) +
                            (b - usedBlue[i]) * (b - usedBlue[i]);
            double dist = qSqrt(distSq);

            if (dist < minDistance)
            {
                tooClose = true;
                break;
            }
        }

        if (!tooClose)
            return;
    }

    // Fallback: use distinct hues
    int index = usedRed.count();
    switch (index % 6)
    {
    case 0:
        r = 255;
        g = 100;
        b = 100;
        break;
    case 1:
        r = 100;
        g = 255;
        b = 100;
        break;
    case 2:
        r = 100;
        g = 100;
        b = 255;
        break;
    case 3:
        r = 255;
        g = 255;
        b = 100;
        break;
    case 4:
        r = 255;
        g = 100;
        b = 255;
        break;
    case 5:
        r = 100;
        g = 255;
        b = 255;
        break;
    }
}

void GenerateTestData::applyRotation(double &x, double &y, double &z,
                                              double angleX, double angleY, double angleZ)
{
    // Rotation around X axis
    double cosX = qCos(angleX), sinX = qSin(angleX);
    double ty = y * cosX - z * sinX;
    double tz = y * sinX + z * cosX;
    y = ty;
    z = tz;

    // Rotation around Y axis
    double cosY = qCos(angleY), sinY = qSin(angleY);
    double tx = x * cosY + z * sinY;
    tz = -x * sinY + z * cosY;
    x = tx;
    z = tz;

    // Rotation around Z axis
    double cosZ = qCos(angleZ), sinZ = qSin(angleZ);
    tx = x * cosZ - y * sinZ;
    ty = x * sinZ + y * cosZ;
    x = tx;
    y = ty;
}
