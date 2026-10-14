#pragma once

#include "Core/CommonCore/Assertion.h"

#include <string>

namespace EngineCore {
struct ComponentData {
    std::string EngineObjectName;
    bool mIsEnabled{true};

    ComponentData(const std::string& gameObjectName, const bool isEnabled = true)
        : EngineObjectName(gameObjectName)
        , mIsEnabled(isEnabled)
    {
        ext_assert(EngineObjectName != "", "ComponentData: EngineObjectName cannot be empty");
    }

    virtual ~ComponentData()
    {
    }
};

} // namespace EngineCore