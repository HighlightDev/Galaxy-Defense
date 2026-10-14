#include "OrbitVelocityModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstdio>

namespace EngineCore {
OrbitVelocityModuleGpuProxy::OrbitVelocityModuleGpuProxy(
    const eOrbitExtraVelocityDirectionType extraVelocityDirectionType, const float speed)
    : mExtraVelocityDirectionType(extraVelocityDirectionType)
    , mExtraVelocityPower(speed)
{
}

std::string OrbitVelocityModuleGpuProxy::GetShaderSnippet() const
{
    const auto fmtStr = R"(
    vec3 updateVelocity(in vec3 initialVelocity, in vec3 currentVelocity, in float particleLifeFactor, in float dt) {
        // Orbit velocity module GPU proxy does not support orbit velocity for now, only extra velocity in the direction of the radius (inside or outside)
        vec3 velocityFromModule = vec3(0.0);
        float initLen = length(initialVelocity);
        if (initLen > 1e-5) {
            vec3 radialDir = initialVelocity / initLen;
            if (%d == 1) { // Inside
                velocityFromModule = -radialDir * %f;
            } else if (%d == 2) { // Outside
                velocityFromModule = radialDir * %f;
            }
        }
        return velocityFromModule;
    }
    vec3 resetVelocity(in float dt, in vec3 currentInvokeId) {
        // On reset generate a random orbit tangent direction
        const float x = 2.0 * random(vec2(dt * 1000.0 + 1.0, currentInvokeId.x +   0.0)) - 1.0;
        const float y = 2.0 * random(vec2(dt * 1000.0 + 2.0, currentInvokeId.x + 100.0)) - 1.0;
        const float z = 2.0 * random(vec2(dt * 1000.0 + 3.0, currentInvokeId.x + 200.0)) - 1.0;
        float len = length(vec3(x, y, z));
        vec3 radialDir = len > 1e-5 ? vec3(x, y, z) / len : vec3(0.0, 1.0, 0.0);
        vec3 velocityFromModule = vec3(0.0);
        if (%d == 1) { // Inside
            velocityFromModule = -radialDir * %f;
        } else if (%d == 2) { // Outside
            velocityFromModule = radialDir * %f;
        }
        return velocityFromModule;
    }
    )";
    char buffer[2048];
    std::snprintf(
        buffer,
        sizeof(buffer),
        fmtStr,
        static_cast<int>(mExtraVelocityDirectionType),
        mExtraVelocityPower,
        static_cast<int>(mExtraVelocityDirectionType),
        mExtraVelocityPower,
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