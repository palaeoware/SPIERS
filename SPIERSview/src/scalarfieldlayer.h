/**
 * @file
 * Header: Scalar Feild Layer
 *
 * All SPIERSview code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#ifndef SCALARFIELDLAYER_H
#define SCALARFIELDLAYER_H

/**
 * @brief The ScalarFieldLayer class
 */
class ScalarFieldLayer
{
public:
    int *edges;  /* tag indexing into vertex list */
    unsigned char *botData;
    unsigned char *topData;

    ScalarFieldLayer();
};

#endif // SCALARFIELDLAYER_H
