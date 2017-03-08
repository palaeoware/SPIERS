#include "myconnectivityfilter.h"
#include <vtkIdTypeArray.h>
MyConnectivityFilter::MyConnectivityFilter() :  vtkPolyDataConnectivityFilter()
{
    ;
}

int MyConnectivityFilter::GetRegionCount(int i)
{
    return (int) RegionSizes->GetValue(i);
}
