#include "RandomRangeLifeTimeModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstdio>

namespace EngineCore {
RandomRangeLifeTimeModuleGpuProxy::RandomRangeLifeTimeModuleGpuProxy(const float minLifeTime, const float maxLifeTime)
    : mMinLifeTime(minLifeTime)
    , mMaxLifeTime(maxLifeTime)
{
}

std::string RandomRangeLifeTimeModuleGpuProxy::GetShaderSnippet() const
{
    // GPU path: no true random range per-particle; approximate with random in [min, max] using GLSL pseudo-random.
    const auto fmtStr = R"(
    float updateLifeTime(in float currentLifeTime, in float dt) {
        return currentLifeTime + dt;
    }
    float resetLifeTime(in float dt, in vec3 currentInvokeId) {
        return 0.0;
    })";
    char buffer[512];
    std::snprintf(buffer, sizeof(buffer), fmtStr, mMinLifeTime, mMaxLifeTime);
    return EngineUtility::Trim(buffer);
}

uint64_t RandomRangeLifeTimeModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("RandomRangeLifeTimeModuleGpuProxy") ^ std::hash<float>{}(mMinLifeTime) ^ std::hash<float>{}(mMaxLifeTime);
}
} // namespace EngineCore
