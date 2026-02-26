#include "OrbitVelocityModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"

namespace EngineCore {
OrbitVelocityModuleGpuProxy::OrbitVelocityModuleGpuProxy(
    const eOrbitExtraVelocityDirectionType extraVelocityDirectionType, const float extraVelocityPower)
    : mExtraVelocityDirectionType(extraVelocityDirectionType)
    , mExtraVelocityPower(extraVelocityPower)
{
}

std::string OrbitVelocityModuleGpuProxy::GetShaderSnippet() const
{
    return "";
}

constexpr uint64_t OrbitVelocityModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("OrbitVelocityModuleGpuProxy");
}
} // namespace EngineCore