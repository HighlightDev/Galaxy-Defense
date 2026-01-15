#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/CubemapComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class CubemapComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit CubemapComponentLuaProxy(const std::shared_ptr<CubemapComponent>& baseComponent);

    ~CubemapComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
