#pragma once

#include "Core/GameCore/GUI/Common/DividerOrientation.h"
#include "UiItemBase.h"

#include <glm/vec3.hpp>

namespace Graphics::Proxy {
class UiSceneProxyBase;
}

namespace EngineCore::Scripts {
class LuaProxy;
} // namespace EngineCore::Scripts

namespace EngineCore::GUI {
class UiDivider : public UiItemBase {
    glm::vec3 mColor;

    float mOpacity;

    float mLineWidthPx;

    float mEdgeFade;

    eDividerOrientation mOrientation;

    std::shared_ptr<EngineObjectProperty<glm::vec3>> mColorProperty;

    std::shared_ptr<EngineObjectProperty<float>> mOpacityProperty;

public:
    explicit UiDivider(const std::string& name = std::string(""));

    ~UiDivider() override;

    void SetColor(const glm::vec3& color);

    void SetColor(const uint8_t r, const uint8_t g, const uint8_t b);

    void SetColor(const uint32_t hexColor);

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    void SetLineWidthPx(const float widthPx);

    float GetLineWidthPx() const;

    void SetEdgeFade(const float edgeFade);

    void SetDividerOrientation(const eDividerOrientation orientation);

    float GetEdgeFade() const;

    glm::vec3 GetColor() const;

    eDividerOrientation GetDividerOrientation() const;

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
} // namespace EngineCore::GUI