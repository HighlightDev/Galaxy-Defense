#include "SimpleVelocityModuleGpuProxy.h"

#include "Core/CommonCore/Random.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <cstdio>

namespace EngineCore {
SimpleVelocityModuleGpuProxy::SimpleVelocityModuleGpuProxy(
    const glm::vec3& velocityDirection, const glm::vec3& velocityDeviation, const float extraVelocityPower)
    : mVelocityDirection(velocityDirection)
    , mVelocityDeviation(velocityDeviation)
    , mCurrentSpawnVelocityDeviation()
    , mExtraVelocityPower(extraVelocityPower)
{
}

std::string SimpleVelocityModuleGpuProxy::GetShaderSnippet() const
{
    const auto fmtStr = R"(
    vec3 updateVelocity(in vec3 currentVelocity, in float particleLifeFactor, in float dt) {
        vec3 velocityDirection = vec3(%f, %f, %f);
        vec3 velocityDeviation = vec3(%f, %f, %f);
        float extraVelocityPower = %f;

        vec3 currentVelocityFromModule = (velocityDirection * dt * extraVelocityPower) + (velocityDeviation * dt);
        return currentVelocityFromModule;
    })";
    char buffer[1024];
    std::snprintf(
        buffer,
        sizeof(buffer),
        fmtStr,
        mVelocityDirection.x,
        mVelocityDirection.y,
        mVelocityDirection.z,
        mVelocityDeviation.x,
        mVelocityDeviation.y,
        mVelocityDeviation.z,
        mExtraVelocityPower);
    return EngineUtility::Trim(buffer);
}

constexpr uint64_t SimpleVelocityModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleVelocityModuleGpuProxy");
}

} // namespace EngineCore