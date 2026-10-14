#pragma once

#include "Core/GameCore/Components/AudioComponents/SoundComponent.h"
#include "Core/GameCore/ScriptingCore/LuaProxies/ComponentProxies/BaseComponentLuaProxy.h"

namespace EngineCore::Scripts {

class SoundComponentLuaProxy : public BaseComponentLuaProxy {
public:
    explicit SoundComponentLuaProxy(const std::shared_ptr<SoundComponent>& baseComponent);

    ~SoundComponentLuaProxy() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override;
};
} // namespace EngineCore::Scripts