#pragma once

#include "UiItemBase.h"

#include <glm/vec3.hpp>
#include <json/json.hpp>

namespace Graphics {
namespace Proxy {
class UiSceneProxyBase;
}
} // namespace Graphics

namespace EngineCore {
namespace Scripts {
class LuaProxy;
}
} // namespace EngineCore

namespace EngineCore {
class UiCanvas;
namespace GUI {
class UiToggleButton : public UiItemBase {
    glm::vec3 mToggleOffColor;

    glm::vec3 mToggleOnColor;

    float mOpacity;

    bool mIsStateOn;

public:
    explicit UiToggleButton(const bool isInitialStateOn, const std::string& name = std::string(""));

    ~UiToggleButton() override;

    bool IsButtonStateOn() const;

    void ToggleButton();

    void SetToggleOnColor(const glm::vec3& color);

    void SetToggleOffColor(const glm::vec3& color);

    void SetToggleOnColor(const uint32_t hexColor);

    void SetToggleOffColor(const uint32_t hexColor);

    glm::vec3 GetToggleOnColor() const;

    glm::vec3 GetToggleOffColor() const;

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    bool OnPropertiesShouldBeUpdatedOnRenderThread() override;

    bool OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

private:
    bool SyncDataOnRenderThread();

    bool SyncDataOnLuaThread();
};
} // namespace GUI
} // namespace EngineCore