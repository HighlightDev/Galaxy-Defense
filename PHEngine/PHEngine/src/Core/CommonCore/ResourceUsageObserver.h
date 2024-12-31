#pragma once

#include <cstdint>
#include <stdint.h>

class ResourceUsageObserver
{
    int32_t mPid{-1};
    double mLastMemUsageMegabytes{0.0};

    explicit ResourceUsageObserver();

public:
    static ResourceUsageObserver *GetInstance();

    void CollectResourceConsumptionInfo();

    int32_t GetPid() const;

    double GetLastMemoryUsageMegabytes() const;
};
