#pragma once

#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"

namespace EngineCore {
class Component;
}

namespace EngineCore::Scripts {

class BaseComponentLuaProxy : public LuaProxy {
protected:
    std::weak_ptr<::EngineCore::Component> mOwnerComponentWp;

public:
    explicit BaseComponentLuaProxy(const std::shared_ptr<::EngineCore::Component>& ownerComponent);

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;

private:
    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;
};

} // namespace EngineCore::Scripts
