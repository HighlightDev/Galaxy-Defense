#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/PrimitiveComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/SceneComponentLuaProxy.h"

namespace EngineCore::Scripts {

class PrimitiveComponentLuaProxy : public SceneComponentLuaProxy {
public:
    explicit PrimitiveComponentLuaProxy(const std::shared_ptr<PrimitiveComponent>& baseComponent);

    ~PrimitiveComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
