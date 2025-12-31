#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
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

using namespace EngineCore;

namespace EngineCore {
class UiCanvas;

namespace GUI {
class UiLabel : public UiItemBase {
    std::string mText;

    float mOpacity;

    const std::string mFontName;

    int32_t mFontSize;

    glm::ivec2 mTextLineWidthHeight;

    glm::vec3 mTextColor;

    eTextHorizontalAlignmentType mTextHorizontalAlignment{eTextHorizontalAlignmentType::LEFT};

    eTextVerticalAlignmentType mTextVerticalAlignment{eTextVerticalAlignmentType::TOP};

    glm::vec2 mTextNormalizedSize;

    glm::ivec2 mTextScreenSpaceSize;

    std::shared_ptr<EngineObjectProperty<float>> mOpacityProperty;

public:
    explicit UiLabel(const std::string& fontName, const std::string& name = std::string(""));

    ~UiLabel() override;

    void SetText(const std::string& text);

    std::string GetText() const;

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    std::string GetFontName() const;

    glm::ivec2 GetTextLineWidthHeight() const;

    void SetFontSize(const int32_t fontSize);

    int32_t GetFontSize() const;

    void SetTextColor(const glm::vec3& color);

    void SetTextColor(const uint32_t hexColor);

    glm::vec3 GetTextColor() const;

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);

    eTextHorizontalAlignmentType GetTextHorizontalAlignment() const;

    void SetTextVerticalAlignment(const eTextVerticalAlignmentType textVericalAlignment);

    eTextVerticalAlignmentType GetTextVerticalAlignment() const;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

    std::shared_ptr<::EngineCore::Scripts::LuaProxy> ReplicateLuaProxy() override;

    void OnPropertiesShouldBeUpdatedOnRenderThread() override;

    void OnPropertiesShouldBeUpdatedOnLuaThread() override;

    void SyncFromLuaJsonProperties(const std::string& luaJsonPropsStr) override;

    std::string GetUiTypeString() const override;

    void SetTextNormalizedSize(const glm::vec2& size);

    void SetTextScreenSpaceSize(const glm::ivec2& size);

    glm::vec2 GetTextNormalizedSize() const;

    glm::ivec2 GetTextScreenSpaceSize() const;

#ifdef DEBUG
    void UpdateIsHiddenForDebugging(const bool isHiddenForDebugging);
#endif

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

private:
    void SyncDataOnRenderThread();

    void SyncDataOnLuaThread();
};
} // namespace GUI
} // namespace EngineCore