#include "SimpleLifeTimeModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstdio>

namespace EngineCore {
SimpleLifeTimeModuleGpuProxy::SimpleLifeTimeModuleGpuProxy(const float lifeTime)
    : mLifeTime(lifeTime)
{
}

std::string SimpleLifeTimeModuleGpuProxy::GetShaderSnippet() const
{
    const auto fmtStr = R"(
    float updateLifeTime(in float currentLifeTime, in float dt) {
        return currentLifeTime + dt;
    }
    float resetLifeTime(in float dt, in vec3 currentInvokeId) {
        return 0.0;
    })";
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), fmtStr);
    return EngineUtility::Trim(buffer);
}

uint64_t SimpleLifeTimeModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleLifeTimeModuleGpuProxy") ^ std::hash<float>{}(mLifeTime);
}

} // namespace EngineCore
