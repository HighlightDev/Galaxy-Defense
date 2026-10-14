#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/SkyboxComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class SkyboxComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit SkyboxComponentLuaProxy(const std::shared_ptr<SkyboxComponent>& baseComponent);

    ~SkyboxComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
