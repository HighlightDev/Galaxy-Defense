#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/BillboardComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class BillboardComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit BillboardComponentLuaProxy(const std::shared_ptr<BillboardComponent>& baseComponent);

    ~BillboardComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
