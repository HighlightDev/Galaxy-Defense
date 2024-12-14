#include "ResourceUsageObserver.h"

#ifdef __linux__
#include <sys/sysinfo.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <fstream>

void ResourceUsageObserver::CollectResourceConsumptionInfo()
{
#ifdef __linux__
    std::string _1, _2, _3, _4, _5, _6, _7, _8, _9, _10,
        _11, _12, _13, _14, _15, _16;
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
    statStream >> mPid >> _1 >> _2 >> _3 >> _4 >> _5 >> _6 >> _7 >> _8 >> _9 >> _10 >> _11 >> _12 >> utime >>
        stime >> cutime >> cstime >> _13 >> _14 >> _15 >> _16 >> starttime >> vsize >> rss;

    static constexpr double BYTES_IN_MBYTE = 1024 * 1024;
    static const double pageSizeBytes = sysconf(_SC_PAGE_SIZE);
    const double PAGE_SIZE_MB = pageSizeBytes / BYTES_IN_MBYTE;
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
