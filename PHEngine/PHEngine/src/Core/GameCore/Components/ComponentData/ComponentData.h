#pragma once

#include "Core/CommonCore/Assertion.h"

#include <string>

namespace EngineCore {
struct ComponentData {
    std::string EngineObjectName;

    ComponentData(const std::string& gameObjectName)
        : EngineObjectName(gameObjectName)
    {
        ext_assert(EngineObjectName != "", "ComponentData: EngineObjectName cannot be empty");
    }

    virtual ~ComponentData()
    {
    }
};

} // namespace EngineCore