#include "SimpleColorModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <glm/gtx/hash.hpp>

#include <cstdio>

namespace EngineCore {
SimpleColorModuleGpuProxy::SimpleColorModuleGpuProxy(const glm::vec3& colorBegin, const glm::vec3& colorEnd)
    : mColorBegin(colorBegin)
    , mColorEnd(colorEnd)
{
}

std::string SimpleColorModuleGpuProxy::GetShaderSnippet() const
{
    const auto fmtStr =
        R"(
        vec3 updateColor(in vec3 currentColor, in float particleLifeProgress, in float dt) {
        return mix(vec3(%f, %f, %f), vec3(%f, %f, %f), particleLifeProgress);
    }
    )";
    char buffer[256];
    std::snprintf(
        buffer, sizeof(buffer), fmtStr, mColorBegin.r, mColorBegin.g, mColorBegin.b, mColorEnd.r, mColorEnd.g, mColorEnd.b);
    return EngineUtility::Trim(buffer);
}

uint64_t SimpleColorModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleColorModuleGpuProxy") ^ std::hash<glm::vec3>{}(mColorBegin) ^ std::hash<glm::vec3>{}(mColorEnd);
}

} // namespace EngineCore
