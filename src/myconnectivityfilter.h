#ifndef MYCONNECTIVITYFILTER_H
#define MYCONNECTIVITYFILTER_H

#include <vtkPolyDataConnectivityFilter.h>

class MyConnectivityFilter : public vtkPolyDataConnectivityFilter
{
public:
    MyConnectivityFilter();
    int GetRegionCount(int i);
};

#endif // MYCONNECTIVITYFILTER_H
