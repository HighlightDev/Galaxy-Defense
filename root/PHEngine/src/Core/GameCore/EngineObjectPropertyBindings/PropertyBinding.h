#pragma once

#include "Core/GameCore/EnginePropertyType.h"

#include <memory>
#include <string>

namespace EngineCore {

struct PropertyBinding {
protected:
    bool bPropertyConnected = false;
    mutable bool bBindingInitialized = false;

public:
    std::string BindingName;
    std::string EngineObjectName;
    std::string EngineObjectPropertyName;

    PropertyBinding(const std::string& bindingName)
        : BindingName(bindingName)
        , EngineObjectName("")
        , EngineObjectPropertyName("")
    {
    }

    virtual eEnginePropertyBindingType GetBindingType() const = 0;

    bool IsPropertyConnected() const
    {
        return bPropertyConnected;
    }
};
} // namespace EngineCore