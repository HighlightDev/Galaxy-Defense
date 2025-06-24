#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "UiRectangle.h"

#include <glm/vec3.hpp>

namespace EngineCore {
class UiCanvas;
namespace GUI {
class UiTextBlock : public UiRectangle {

    std::string mText;

    float mOpacity{1.0f};

    const std::string mFontName;

    float mFontSize{11.0f};

    float mTextLineWidth{1.0f};

    glm::vec3 mTextColor;

    eTextHorizontalAlignmentType mTextHorizontalAlignment{eTextHorizontalAlignmentType::LEFT};

public:
    explicit UiTextBlock(const std::string& fontName, const std::string& name = std::string(""));

    ~UiTextBlock() override;

    void SetText(const std::string& text);

    std::string GetText() const;

    void SetOpacity(const float opacity);

    float GetOpacity() const;

    std::string GetFontName() const;

    float GetTextLineWidth() const;

    void SetFontSize(const float fontSize);

    float GetFontSize() const;

    void SetTextColor(const glm::vec3& color);

    void SetTextColor(const uint32_t hexColor);

    glm::vec3 GetTextColor() const;

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);

    eTextHorizontalAlignmentType GetTextHorizontalAlignment() const;

    std::shared_ptr<::Graphics::Proxy::UiSceneProxyBase> CreateUiSceneProxy() const override;

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