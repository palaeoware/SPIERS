/**
 * @file
 * Header: Data Connectivity Filter (stub — VTK removed)
 *
 * All SPIERSview code is released under the GNU General Public License.
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

#ifndef DATACONNECTIVITYFILTER_H
#define DATACONNECTIVITYFILTER_H

/**
 * @brief Stub replacement for the old vtkPolyDataConnectivityFilter subclass.
 *
 * The island-removal logic that used this class is a no-op stub in svobject.cpp,
 * so this class just needs to exist and compile. All methods return safe defaults.
 */
class DataConnectivityFilter
{
public:
    DataConnectivityFilter();
    int GetRegionCount(int i);
};

#endif // DATACONNECTIVITYFILTER_H