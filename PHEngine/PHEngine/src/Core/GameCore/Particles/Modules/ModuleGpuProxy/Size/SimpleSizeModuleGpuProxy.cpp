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
    char buffer[512];
    std::snprintf(buffer, sizeof(buffer), fmtStr, mSizeBegin, mSizeEnd);
    return EngineUtility::Trim(buffer);
}

constexpr uint64_t SimpleSizeModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleSizeModuleGpuProxy");
}

} // namespace EngineCore
