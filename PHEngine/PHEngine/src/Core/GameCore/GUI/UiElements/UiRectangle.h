#pragma once

#include "UiItemBase.h"

#include <glm/vec3.hpp>

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
class UiRectangle : public UiItemBase {
    glm::vec3 mColor;

    float mOpacity;

    float mBorderRadius;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mColorProperty;

    std::shared_ptr<EngineObjectProperty<float>> mOpacityProperty;

public:
    explicit UiRectangle(const std::string& name = std::string(""));

    ~UiRectangle() override;

    void SetColor(const glm::vec3& color);

    void SetColor(const uint8_t r, const uint8_t g, const uint8_t b);

    void SetColor(const uint32_t hexColor);

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    void SetBorderRadius(const float radiusPx);

    float GetBorderRadius() const;

    glm::vec3 GetColor() const;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void OnPropertiesShouldBeUpdatedOnRenderThread() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

private:
    void SyncDataOnRenderThread();

    void SyncDataOnLuaThread();
};
} // namespace GUI
} // namespace EngineCore