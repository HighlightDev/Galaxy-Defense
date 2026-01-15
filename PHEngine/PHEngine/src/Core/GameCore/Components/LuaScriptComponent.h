#pragma once

#include "Component.h"

namespace EngineCore {

namespace Scripts {
class LuaEngineScriptExecutor;
} // namespace Scripts

class ComponentData;
class LuaScriptComponent : public Component {

    std::shared_ptr<Scripts::LuaEngineScriptExecutor> mLuaScriptExecutor;

    std::string mScriptName;

public:
    LuaScriptComponent(const std::shared_ptr<ComponentData>& componentData);

    virtual ~LuaScriptComponent() override = default;

    eComponentType GetComponentType() const override;

    void Tick(const float deltaTimeSec) override;

private:
    void OnRegistered() override;
};
} // namespace EngineCore
