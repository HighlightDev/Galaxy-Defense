#pragma once

#include "Core/GameCore/GUI/Common/TextHorizontalAlignmentType.h"
#include "UiItemBaseLuaProxy.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>

namespace EngineCore {
namespace GUI {
class UiLabel;
}
} // namespace EngineCore

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
class UiLabelLuaProxy : public UiItemBaseLuaProxy {
protected:
    std::string mText;

    float mOpacity;

    const std::string mFontName;

    int32_t mFontSize;

    glm::ivec2 mTextLineWidthHeight;

    glm::vec3 mTextColor;

    eTextHorizontalAlignmentType mTextHorizontalAlignment;

public:
    explicit UiLabelLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiLabel>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetText_FromGameThread(const std::string& text);

    void SetOpacity_FromGameThread(const float opacity);

    void SetFontSize_FromGameThread(const int32_t fontSize);

    void SetTextLineWidthHeight_FromGameThread(const glm::ivec2& textLineWidthHeight);

    void SetTextColor_FromGameThread(const glm::vec3& texColor);

    void SetTextHorizontalAlignment(const eTextHorizontalAlignmentType textHorizontalAlignment);
};
} // namespace Scripts
} // namespace EngineCore
