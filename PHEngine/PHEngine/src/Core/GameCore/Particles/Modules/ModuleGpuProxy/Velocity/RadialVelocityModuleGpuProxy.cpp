#include "RadialVelocityModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstdio>

using namespace EngineCore;

namespace EngineCore {
RadialVelocityModuleGpuProxy::RadialVelocityModuleGpuProxy(const float speed)
    : mSpeed(speed)
{
}

std::string RadialVelocityModuleGpuProxy::GetShaderSnippet() const
{
    const auto snippet = R"(
    vec3 updateVelocity(in vec3 initialVelocity, in vec3 currentVelocity, in float particleLifeFactor, in float dt) {
        float speed = %f;
        return initialVelocity * speed;
    }
    vec3 resetVelocity(in float dt, in vec3 currentInvokeId) {
        float speed = %f;
        const float x = 2.0 * random(vec2(dt * 1000.0 + 1.0, currentInvokeId.x +   0.0)) - 1.0;
        const float y = 2.0 * random(vec2(dt * 1000.0 + 2.0, currentInvokeId.x + 100.0)) - 1.0;
        const float z = 2.0 * random(vec2(dt * 1000.0 + 3.0, currentInvokeId.x + 200.0)) - 1.0;
        float len = length(vec3(x, y, z));
        vec3 radialDir = len > 1e-5 ? vec3(x, y, z) / len : vec3(0.0, 1.0, 0.0);
        return radialDir * speed;
    }
    )";
    char buffer[1024];
    std::snprintf(buffer, sizeof(buffer), snippet, mSpeed, mSpeed);
    return EngineUtility::Trim(buffer);
}

uint64_t RadialVelocityModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("RadialVelocityModuleGpuProxy") ^ std::hash<float>{}(mSpeed);
}
} // namespace EngineCore
