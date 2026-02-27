#include "OrbitVelocityModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstdio>

namespace EngineCore {
OrbitVelocityModuleGpuProxy::OrbitVelocityModuleGpuProxy(
    const eOrbitExtraVelocityDirectionType extraVelocityDirectionType, const float extraVelocityPower)
    : mExtraVelocityDirectionType(extraVelocityDirectionType)
    , mExtraVelocityPower(extraVelocityPower)
{
}

std::string OrbitVelocityModuleGpuProxy::GetShaderSnippet() const
{
    const auto fmtStr = R"(
    vec3 updateVelocity(in vec3 currentVelocity, in float particleLifeFactor, in float dt) {
        // Orbit velocity module GPU proxy does not support orbit velocity for now, only extra velocity in the direction of the radius (inside or outside)
        vec3 velocityFromModule = vec3(0.0);
        if (%d == 1) { // Inside
            velocityFromModule = -normalize(currentVelocity) * %f;
        } else if (%d == 2) { // Outside
            velocityFromModule = normalize(currentVelocity) * %f;
        }
        return velocityFromModule;
    }
    )";
    char buffer[1024];
    std::snprintf(
        buffer,
        sizeof(buffer),
        fmtStr,
        static_cast<int>(mExtraVelocityDirectionType),
        mExtraVelocityPower,
        static_cast<int>(mExtraVelocityDirectionType),
        mExtraVelocityPower);
    return EngineUtility::Trim(buffer);
}

uint64_t OrbitVelocityModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("OrbitVelocityModuleGpuProxy") ^ std::hash<int>()(static_cast<int>(mExtraVelocityDirectionType))
        ^ std::hash<float>{}(mExtraVelocityPower);
}
} // namespace EngineCore