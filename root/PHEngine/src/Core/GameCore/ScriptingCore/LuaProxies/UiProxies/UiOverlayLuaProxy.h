#pragma once

#include "Core/GameCore/ScriptingCore/LuaProxies/LuaProxy.h"

#include <memory>
#include <string>

namespace EngineCore {
namespace GUI {
class UiOverlay;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiOverlayLuaProxy : public LuaProxy {
    std::string mOverlayName;

public:
    explicit UiOverlayLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiOverlay>& owner);

    std::string GetOverlayName() const;

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    bool InvokeFunction(const std::string& functionName, const std::string& jsonParameters) override
    {
        // No functions to invoke for now
        return false;
    }
};
} // namespace Scripts
} // namespace EngineCore
