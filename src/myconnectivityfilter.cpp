#include "myconnectivityfilter.h"
#include <vtkIdTypeArray.h>

/**
 * @brief MyConnectivityFilter::MyConnectivityFilter
 */
MyConnectivityFilter::MyConnectivityFilter() :  vtkPolyDataConnectivityFilter()
{
    ;
}

/**
 * @brief MyConnectivityFilter::GetRegionCount
 * @param i
 * @return
 */
int MyConnectivityFilter::GetRegionCount(int i)
{
    return static_cast<int>(RegionSizes->GetValue(i));
}
