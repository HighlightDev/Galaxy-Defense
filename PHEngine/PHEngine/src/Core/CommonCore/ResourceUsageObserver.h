#pragma once

#include <string>

class ResourceUsageObserver
{
    int32_t mPid{-1};
    double mLastMemUsageMegabytes{0.0};

public:
    void CollectResourceConsumptionInfo();
    
    int32_t GetPid() const;

    double GetLastMemoryUsageMegabytes() const;
};
