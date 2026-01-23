#pragma once

#include "Core/GameCore/Components/ParticleComponents/CpuParticleSystemComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class ParticleSystemComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit ParticleSystemComponentLuaProxy(const std::shared_ptr<CpuParticleSystemComponent>& baseComponent);

    ~ParticleSystemComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
