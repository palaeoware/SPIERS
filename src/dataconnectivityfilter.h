#ifndef DATACONNECTIVITYFILTER_H
#define DATACONNECTIVITYFILTER_H

#include "vtkPolyDataConnectivityFilter.h"

class DataConnectivityFilter : public vtkPolyDataConnectivityFilter
{
public:
    DataConnectivityFilter();
    int GetRegionCount(int i);
};

#endif // DATACONNECTIVITYFILTER_H
