#pragma once

#include "Core/CommonCore/Assertion.h"

#include <string>

namespace EngineCore {
struct ComponentData {
    std::string EngineObjectName;

    ComponentData(const std::string& gameObjectName)
        : EngineObjectName(gameObjectName)
    {
        assert(EngineObjectName != "");
    }

    virtual ~ComponentData()
    {
    }
};

} // namespace EngineCore