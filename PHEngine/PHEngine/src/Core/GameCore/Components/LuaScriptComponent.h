#pragma once

#include "Component.h"
#include "ComponentData/ComponentData.h"
#include "ComponentData/ScriptComponentData.h"

namespace EngineCore {

namespace Scripts {
class LuaEngineScriptExecutor;
} // namespace Scripts

class ComponentData;
class LuaScriptComponent : public Component {

    std::shared_ptr<Scripts::LuaEngineScriptExecutor> mLuaScriptExecutor;

    std::string mScriptName;

public:
    explicit LuaScriptComponent(const std::shared_ptr<EngineCore::ComponentData>& componentData)
        : Component(componentData->EngineObjectName, componentData->mIsEnabled)
        , mScriptName(std::static_pointer_cast<EngineCore::ScriptComponentData>(componentData)->ScriptName)
    {
    }

    ~LuaScriptComponent() override = default;

    eComponentType GetComponentType() const override;

    void Tick(const float deltaTimeSec, const float playSpeed) override;

private:
    void OnRegistered() override;
};
} // namespace EngineCore
