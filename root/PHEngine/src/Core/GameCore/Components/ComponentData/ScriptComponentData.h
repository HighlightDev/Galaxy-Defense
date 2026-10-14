#pragma once

#include "Core/CommonCore/Assertion.h"
#include "Core/GameCore/Components/ComponentData/ComponentData.h"

#include <string>

namespace EngineCore {
struct ScriptComponentData : ComponentData {
    std::string ScriptName;

    ScriptComponentData(const std::string& gameObjectName, const std::string& scriptName, const bool isEnabled = true)
        : ComponentData(gameObjectName, isEnabled)
        , ScriptName(scriptName)
    {
    }
};

} // namespace EngineCore