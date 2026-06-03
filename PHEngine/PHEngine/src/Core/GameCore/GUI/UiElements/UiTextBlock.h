#pragma once

#include "Core/GameCore/GUI/Common/TextEnums.h"
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

    std::string mAttachTargetUiItemName;

    int32_t mBorderThickness{1};

    glm::vec2 mTextNormalizedSize;

    glm::ivec2 mTextScreenSpaceSize;

    eTextGradientColorType mTextGradientColorType{eTextGradientColorType::NONE};

    glm::vec3 mGradientTextColorStart;

    glm::vec3 mGradientTextColorEnd;

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

    eTextGradientColorType GetTextGradientColorType() const;

    void SetTextGradientColorType(const eTextGradientColorType textGradientColorType);

    glm::vec3 GetGradientTextColorStart() const;

    glm::vec3 GetGradientTextColorEnd() const;

    void SetGradientTextColors(const glm::vec3& gradientTextColorStart, const glm::vec3& gradientTextColorEnd);

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

    void SetAttachTargetUiItemName(const std::string& uiItemName);

    std::string GetAttachTargetUiItemName() const;

    void SetBorderThickness(const int32_t thickness);

    int32_t GetBorderThickness() const;

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

protected:
    void OnRegistered() override;

    void OnUnregistered() override;

    void RecalculateAnchorPositions() override;

private:
    void SyncDataOnRenderThread();

    void SyncDataOnLuaThread();

    void RecalculatePositionAccordingToAttachTarget();

    glm::ivec2 FindFreeAttachPosition(const BoundingBox2D<glm::ivec2>& targetUiBoundingArea) const;
};
} // namespace GUI
} // namespace EngineCore