#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/InstancedStaticMeshComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class InstancedStaticMeshComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit InstancedStaticMeshComponentLuaProxy(const std::shared_ptr<InstancedStaticMeshComponent>& baseComponent);

    ~InstancedStaticMeshComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
