#include "SimpleLifeTimeModuleGpuProxy.h"

#include "Core/CommonCore/StringHash.h"

namespace EngineCore {
SimpleLifeTimeModuleGpuProxy::SimpleLifeTimeModuleGpuProxy(const float lifeTime)
    : mLifeTime(lifeTime)
{
}

std::string SimpleLifeTimeModuleGpuProxy::GetShaderSnippet() const
{
    return "";
}

constexpr uint64_t SimpleLifeTimeModuleGpuProxy::GetModuleTypeHash() const
{
    return Hash64_CT("SimpleLifeTimeModuleGpuProxy");
}

} // namespace EngineCore
