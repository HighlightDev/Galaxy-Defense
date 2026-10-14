#pragma once

#include <stdint.h>

#include <cstdint>

class ResourceUsageObserver {
    int32_t mPid{-1};
    double mLastMemUsageMegabytes{0.0};

    explicit ResourceUsageObserver();

public:
    static ResourceUsageObserver* GetInstance();

    void CollectResourceConsumptionInfo();

    int32_t GetPid() const;

    double GetLastMemoryUsageMegabytes() const;
};
