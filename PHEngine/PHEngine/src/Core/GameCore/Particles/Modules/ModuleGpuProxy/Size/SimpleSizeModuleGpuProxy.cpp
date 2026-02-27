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
    )";
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), fmtStr, mSizeBegin, mSizeEnd);
    return EngineUtility::Trim(buffer);
}

uint64_t SimpleSizeModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleSizeModuleGpuProxy") ^ std::hash<float>{}(mSizeBegin) ^ std::hash<float>{}(mSizeEnd);
}

} // namespace EngineCore
