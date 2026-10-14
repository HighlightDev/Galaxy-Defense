#pragma once

#include "UiItemBase.h"

#include <glm/mat4x4.hpp>

#include <memory>
#include <unordered_map>
#include <vector>

namespace EngineCore {
namespace Scripts {
class LuaProxy;
}
} // namespace EngineCore

namespace EngineCore {
namespace GUI {
class UiCanvas;

class UiItem : public UiItemBase {
public:
    UiItem(const std::string& name = std::string(""));

    void OnRegistered() override;

    void OnUnregistered() override;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override
    {
        return nullptr;
    }

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    std::string GetUiTypeString() const override;
};
} // namespace GUI
} // namespace EngineCore