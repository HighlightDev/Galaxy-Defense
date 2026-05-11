#include "SimpleVelocityModuleGpuProxy.h"

#include "Core/CommonCore/Random.h"
#include "Core/CommonCore/StringHash.h"
#include "Core/UtilityCore/StringExtendedFunctions.h"

#include <glm/gtx/hash.hpp>

#include <cstdio>

namespace EngineCore {
SimpleVelocityModuleGpuProxy::SimpleVelocityModuleGpuProxy(
    const glm::vec3& velocityDirection, const glm::vec3& velocityDeviation, const float speed)
    : mVelocityDirection(velocityDirection)
    , mVelocityDeviation(velocityDeviation)
    , mCurrentSpawnVelocityDeviation()
    , mSpeed(speed)
{
}

std::string SimpleVelocityModuleGpuProxy::GetShaderSnippet() const
{
    const auto fmtStr = R"(
    vec3 updateVelocity(in vec3 initialVelocity, in vec3 currentVelocity, in float particleLifeFactor, in float dt) {
        vec3 velocityDirection = vec3(%f, %f, %f);
        vec3 velocityDeviation = vec3(%f, %f, %f);
        float speed = %f;

        vec3 currentVelocityFromModule = normalize(velocityDirection + velocityDeviation + initialVelocity) * speed;
        return currentVelocityFromModule;
    }
    vec3 resetVelocity(in float dt, in vec3 currentInvokeId) {
        vec3 velocityDirection = vec3(%f, %f, %f);
        vec3 velocityDeviation = vec3(%f, %f, %f);
        float speed = %f;

        const float signX = random(vec2(dt * 1000.0,        currentInvokeId.x +   0.0));
        const float signY = random(vec2(dt * 1000.0 + 10.0,  currentInvokeId.x + 300.0));
        const float signZ = random(vec2(dt * 1000.0 + 20.0,  currentInvokeId.x + 400.0));
        vec3 velocityDeviationFromModule = normalize(vec3(signX, signY, signZ) * velocityDeviation);

        const float initX = 2.0 * random(vec2(dt * 1000.0 + 1.0, currentInvokeId.x +   0.0)) - 1.0;
        const float initY = 2.0 * random(vec2(dt * 1000.0 + 2.0, currentInvokeId.x + 100.0)) - 1.0;
        const float initZ = 2.0 * random(vec2(dt * 1000.0 + 3.0, currentInvokeId.x + 200.0)) - 1.0;
        vec3 initialVelocityFromModule = normalize(vec3(initX * 2.0f, initY * 2.0f, initZ * 2.0f));

        return normalize(velocityDirection + velocityDeviationFromModule + initialVelocityFromModule) * speed;
    }
    )";
    char buffer[16384];
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
        mSpeed,
        mVelocityDirection.x,
        mVelocityDirection.y,
        mVelocityDirection.z,
        mVelocityDeviation.x,
        mVelocityDeviation.y,
        mVelocityDeviation.z,
        mSpeed);
    return EngineUtility::Trim(buffer);
}

uint64_t SimpleVelocityModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleVelocityModuleGpuProxy") ^ std::hash<glm::vec3>{}(mVelocityDirection)
        ^ std::hash<glm::vec3>{}(mVelocityDeviation) ^ std::hash<float>{}(mSpeed);
}

} // namespace EngineCore