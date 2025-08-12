#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "UiRectangleLuaProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiTextBlock;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiTextBlockLuaProxy : public UiRectangleLuaProxy {
protected:
    std::string mText;

    float mOpacity;

    const std::string mFontName;

    int32_t mFontSize;

    float mTextLineWidth;

    glm::vec3 mTextColor;

    eTextHorizontalAlignmentType mTextHorizontalAlignment;

public:
    explicit UiTextBlockLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiTextBlock>& ownerTextBlock);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetText_FromGameThread(const std::string& text);

    void SetOpacity_FromGameThread(const float opacity);

    void SetFontSize_FromGameThread(const int32_t fontSize);

    void SetTextLineWidth_FromGameThread(const float textLineWidth);

    void SetTextColor_FromGameThread(const glm::vec3& texColor);

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);
};
} // namespace Scripts
} // namespace EngineCore
