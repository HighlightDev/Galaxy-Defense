#pragma once

#include "Core/GameCore/Components/PrimitiveComponents/SkeletalMeshComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/PrimitiveComponentLuaProxy.h"

namespace EngineCore::Scripts {

class SkeletalMeshComponentLuaProxy : public PrimitiveComponentLuaProxy {
public:
    explicit SkeletalMeshComponentLuaProxy(const std::shared_ptr<SkeletalMeshComponent>& baseComponent);

    ~SkeletalMeshComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts
