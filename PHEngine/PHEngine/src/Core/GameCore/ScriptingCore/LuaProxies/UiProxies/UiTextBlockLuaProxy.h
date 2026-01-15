#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "UiItemBaseLuaProxy.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiTextBlock;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiTextBlockLuaProxy : public UiItemBaseLuaProxy {
protected:
    std::string mText;

    float mOpacity;

    const std::string mFontName;

    int32_t mFontSize;

    glm::ivec2 mTextLineWidthHeight;

    glm::vec3 mTextColor;

    eTextHorizontalAlignmentType mTextHorizontalAlignment;

    eTextVerticalAlignmentType mTextVerticalAlignment;

    glm::vec3 mRectangleColor;

    float mRectangleOpacity;

    float mRectangleRadius;

    glm::vec3 mBorderColor;

    float mBorderRadius;

    float mBorderOpacity;

    std::string mAttachTargetUiItemName;

    int32_t mBorderThickness;

public:
    explicit UiTextBlockLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiTextBlock>& ownerTextBlock);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetText_FromGameThread(const std::string& text);

    void SetOpacity_FromGameThread(const float opacity);

    void SetFontSize_FromGameThread(const int32_t fontSize);

    void SetTextLineWidthHeight_FromGameThread(const glm::ivec2& textLineWidthHeight);

    void SetTextColor_FromGameThread(const glm::vec3& texColor);

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);

    void SetTextVerticalAlignment(const eTextVerticalAlignmentType textVerticalAlignment);

    void SetRectangleColor_FromGameThread(const glm::vec3& color);

    void SetRectangleOpacity_FromGameThread(const float opacity);

    void SetRectangleBorderRadius_FromGameThread(const float borderRadius);

    void SetBorderColor_FromGameThread(const glm::vec3& color);

    void SetBorderRadius_FromGameThread(const float borderRadius);

    void SetBorderOpacity_FromGameThread(const float opacity);

    void SetAttachTargetUiItemName_FromGameThread(const std::string& uiItemName);

    void SetBorderThickness_FromGameThread(const int32_t thickness);
};
} // namespace Scripts
} // namespace EngineCore
