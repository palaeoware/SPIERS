/**
 * @file
 * Header: Generate Test Data
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

#ifndef GENERATETESTDATA_H
#define GENERATETESTDATA_H

#include <QWidget>
#include <QImage>
#include <QList>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QRadioButton>
#include <QLabel>

/**
 * @brief Generates synthetic 3D tomographic datasets for testing purposes.
 *
 * This widget provides controls to create test image stacks with randomized
 * 3D geometric shapes (rectangles, circles, triangles) with distinct gray/RGB values.
 * Generated images are saved as numbered sequences (test_0, test_1, etc.).
 */
class GenerateTestData : public QWidget
{
    Q_OBJECT

public:
    /**
     *
     * Constructs the Generate Test Data widget and sets up UI controls.
     *
     **/
    GenerateTestData(QWidget *parent = nullptr);

    /**
     *
     * Destructor.
     *
     **/
    ~GenerateTestData();

private slots:
    /**
     *
     * Triggered when the Generate button is clicked. Opens a folder selection
     * dialog and initiates dataset generation.
     *
     **/
    void on_GenerateButton_clicked();

    /**
     *
     * Triggered when background color button is clicked. Opens color dialog.
     *
     **/
    void on_BgColorButton_clicked();

private:
    /**
     *
     * Represents a 3D geometric shape to be rendered in the dataset.
     *
     **/
    struct Shape
    {
        int type;                /// 0=rectangle, 1=circle, 2=triangle
        double centerX, centerY, centerZ; /// Center position in 3D space
        double sizeX, sizeY, sizeZ;      /// Dimensions
        double angleX, angleY, angleZ;   /// Rotation angles in radians
        uchar grayValue;         /// Grayscale value (0-255)
        uchar r, g, b;           /// RGB values
    };

    /**
     *
     * Generates the complete dataset at the given output path. Opens a progress
     * dialog and iterates through Z slices, rendering shapes and saving images.
     *
     **/
    void generateDataset(const QString &outputPath);

    /**
     *
     * Renders all shapes onto the given image for slice at index @p sliceIndex.
     * Iterates through pixels, checking if each is inside any shape.
     * If @p allowIntersections is false, only the first shape at each pixel is rendered.
     *
     **/
    void renderSlice(QImage &image, int sliceIndex, const QList<Shape> &shapes, bool allowIntersections);

    /**
     *
     * Determines if a pixel at (@p x, @p y) on slice @p z is inside the given @p shape.
     * Uses 3D distance calculations and rotation matrix transformations.
     *
     **/
    bool isPixelInShape(int x, int y, int z, const Shape &shape);

    /**
     *
     * Generates a list of randomized shapes based on current UI settings.
     * Each shape is assigned a distinct gray/RGB value and random position/angle.
     *
     **/
    QList<Shape> generateRandomShapes();

    /**
     *
     * Selects a distinct grayscale value not in @p usedValues, avoiding
     * very dark (0-30) and very light (220-255) ranges to maintain contrast.
     *
     **/
    uchar selectDistinctGrayValue(const QList<uchar> &usedValues);

    /**
     *
     * Selects distinct RGB values with high contrast from background and existing shapes.
     * Ensures Euclidean distance from other colors meets minimum threshold.
     *
     **/
    void selectDistinctRGBValue(const QList<uchar> &usedRed,
                                const QList<uchar> &usedGreen,
                                const QList<uchar> &usedBlue,
                                uchar &r, uchar &g, uchar &b);

    /**
     *
     * Returns rotation matrix for 3D rotation by angles (@p angleX, @p angleY, @p angleZ).
     * Used to transform points when checking if pixel is inside rotated shape.
     *
     **/
    void applyRotation(double &x, double &y, double &z,
                       double angleX, double angleY, double angleZ);

    // UI Controls
    QSpinBox *dimXSpinBox;       /// X dimension (width)
    QSpinBox *dimYSpinBox;       /// Y dimension (height)
    QSpinBox *dimZSpinBox;       /// Z dimension (number of slices)
    QComboBox *formatComboBox;   /// Output format (.jpg or .png)
    QRadioButton *grayscaleRadio; /// Grayscale mode selector
    QRadioButton *rgbRadio;      /// RGB mode selector
    QSpinBox *numObjectsSpinBox; /// Number of shapes (default 6, max 6)
    QCheckBox *allowIntersectionCheckBox; /// Allow shapes to overlap
    QPushButton *bgColorButton;  /// Background color selector button
    QLabel *bgColorLabel;        /// Background color display
    QLabel *bgColorDisplayLabel; /// Color swatch display
    QPushButton *generateButton; /// Generate dataset button

    QColor backgroundColor;      /// Current background color (default black)
};

#endif // GENERATETESTDATAL_H
