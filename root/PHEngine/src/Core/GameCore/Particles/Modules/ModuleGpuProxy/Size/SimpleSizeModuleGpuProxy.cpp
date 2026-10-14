#include "SimpleSizeModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstdio>

namespace EngineCore {
SimpleSizeModuleGpuProxy::SimpleSizeModuleGpuProxy(const float sizeBegin, const float sizeEnd)
    : mSizeBegin(sizeBegin)
    , mSizeEnd(sizeEnd)
{
}

std::string SimpleSizeModuleGpuProxy::GetShaderSnippet() const
{
    const auto fmtStr =
        R"(
        vec2 updateRotationAndSize(in vec2 currentRotationAndSize, in float particleLifeProgress, in float dt) {
        return vec2(currentRotationAndSize.x, mix(%f, %f, particleLifeProgress));
    }
    vec2 resetRotationAndSize(in float dt, in vec3 currentInvokeId) {
        float rotation = random(vec2(dt * 1000.0, currentInvokeId.x)) * 6.28318;
        return vec2(rotation, %f);
    }
    )";
    char buffer[512];
    std::snprintf(buffer, sizeof(buffer), fmtStr, mSizeBegin, mSizeEnd, mSizeBegin);
    return EngineUtility::Trim(buffer);
}

uint64_t SimpleSizeModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleSizeModuleGpuProxy") ^ std::hash<float>{}(mSizeBegin) ^ std::hash<float>{}(mSizeEnd);
}

} // namespace EngineCore
