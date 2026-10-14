#pragma once

#include "Core/GameCore/Components/SceneComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/BaseComponentLuaProxy.h"

namespace EngineCore::Scripts {

class SceneComponentLuaProxy : public BaseComponentLuaProxy {
public:
    explicit SceneComponentLuaProxy(const std::shared_ptr<SceneComponent>& baseComponent);

    ~SceneComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts