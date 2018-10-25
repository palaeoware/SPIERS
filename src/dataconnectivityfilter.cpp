#include "dataconnectivityfilter.h"
#include "vtkIdTypeArray.h"

/**
 * @brief DataConnectivityFilter::DataConnectivityFilter
 */
DataConnectivityFilter::DataConnectivityFilter() :  vtkPolyDataConnectivityFilter()
{
    ;
}

/**
 * @brief DataConnectivityFilter::GetRegionCount
 * @param i
 * @return
 */
int DataConnectivityFilter::GetRegionCount(int i)
{
    return static_cast<int>(RegionSizes->GetValue(i));
}
