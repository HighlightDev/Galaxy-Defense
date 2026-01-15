#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/ElectricBeamComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class ElectricBeamComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit ElectricBeamComponentLuaProxy(const std::shared_ptr<ElectricBeamComponent>& baseComponent);

    ~ElectricBeamComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
