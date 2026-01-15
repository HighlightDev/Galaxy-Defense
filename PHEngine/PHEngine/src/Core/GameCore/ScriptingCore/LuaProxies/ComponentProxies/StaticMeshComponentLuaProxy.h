#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/StaticMeshComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class StaticMeshComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit StaticMeshComponentLuaProxy(const std::shared_ptr<StaticMeshComponent>& baseComponent);

    ~StaticMeshComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
