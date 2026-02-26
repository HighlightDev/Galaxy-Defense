#include "ExplosionInitialVelocityModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"

namespace EngineCore {
ExplosionInitialVelocityModuleGpuProxy::ExplosionInitialVelocityModuleGpuProxy()
{
}

std::string ExplosionInitialVelocityModuleGpuProxy::GetShaderSnippet() const
{
    return "";
}

constexpr uint64_t ExplosionInitialVelocityModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("ExplosionInitialVelocityModuleGpuProxy");
}

} // namespace EngineCore