#include "ResourceUsageObserver.h"

#ifdef _WIN32
#include <psapi.h>
#include <wchar.h>
#include <windows.h>
#pragma comment(lib, "Psapi.lib")
#elif defined(__linux__)
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <fstream>

ResourceUsageObserver::ResourceUsageObserver()
{
#ifdef _WIN32
    mPid = static_cast<int32_t>(GetCurrentProcessId());
#endif
}

ResourceUsageObserver* ResourceUsageObserver::GetInstance()
{
    static ResourceUsageObserver s_instance;
    return &s_instance;
}

void ResourceUsageObserver::CollectResourceConsumptionInfo()
{
    static constexpr double BYTES_IN_MBYTE = 1024 * 1024;
    static constexpr double INV_BYTES_IN_MBYTE = 1.0 / BYTES_IN_MBYTE;
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    uint64_t mHeapCapacity = static_cast<uint64_t>(pmc.WorkingSetSize);
    mLastMemUsageMegabytes = static_cast<double>(mHeapCapacity) * INV_BYTES_IN_MBYTE;
#elif __linux__
    std::string _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16;
    // RAM
    int64_t rss;
    uint64_t vsize;

    // CPU
    uint32_t utime;
    uint32_t stime;
    uint32_t cutime;
    uint32_t cstime;
    uint64_t starttime;

    std::ifstream statStream("/proc/self/stat");
    statStream >> mPid >> _1 >> _2 >> _3 >> _4 >> _5 >> _6 >> _7 >> _8 >> _9 >> _10 >> _11 >> _12 >> utime >> stime >> cutime
        >> cstime >> _13 >> _14 >> _15 >> _16 >> starttime >> vsize >> rss;

    static const double pageSizeBytes = sysconf(_SC_PAGE_SIZE);
    const double PAGE_SIZE_MB = pageSizeBytes * INV_BYTES_IN_MBYTE;
    mLastMemUsageMegabytes = static_cast<double>(rss) * PAGE_SIZE_MB;
#endif
}

int32_t ResourceUsageObserver::GetPid() const
{
    return mPid;
}

double ResourceUsageObserver::GetLastMemoryUsageMegabytes() const
{
    return mLastMemUsageMegabytes;
}
