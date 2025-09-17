#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "UiItemBase.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EngineCore {
class UiCanvas;
namespace GUI {
class UiTextBlock : public UiItemBase {

    std::string mText;

    float mOpacity{1.0f};

    const std::string mFontName;

    int32_t mFontSize{16};

    glm::ivec2 mTextLineWidthHeight;

    glm::vec3 mTextColor;

    eTextHorizontalAlignmentType mTextHorizontalAlignment{eTextHorizontalAlignmentType::LEFT};

    eTextVerticalAlignmentType mTextVerticalAlignment{eTextVerticalAlignmentType::TOP};

    glm::vec3 mRectangleColor;

    float mRectangleOpacity{1.0f};

    float mRectangleRadius;

    glm::vec3 mBorderColor;

    float mBorderRadius;

    float mBorderOpacity{1.0f};

public:
    explicit UiTextBlock(const std::string& fontName, const std::string& name = std::string(""));

    ~UiTextBlock() override;

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

    void SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment);

    eTextVerticalAlignmentType GetTextVerticalAlignment() const;

    void SetRectangleColor(const glm::vec3& color);

    void SetRectangleOpacity(const float opacity);

    void SetRectangleRadius(const float borderRadius);

    glm::vec3 GetRectangleColor() const;

    float GetRectangleOpacity() const;

    float GetRectangleRadius() const;

    void SetBorderColor(const glm::vec3& color);

    void SetBorderRadius(const float borderRadius);

    void SetBorderOpacity(const float opacity);

    float GetBorderOpacity() const;

    glm::vec3 GetBorderColor() const;

    float GetBorderRadius() const;

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