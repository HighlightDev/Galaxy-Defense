#pragma once

#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiProgressBar;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiProgressBarLuaProxy : public UiItemBaseLuaProxy {
protected:
    glm::vec3 mEmptyColor;

    glm::vec3 mFilledColor;

    float mOpacity;

    float mFillPercentValue;

public:
    explicit UiProgressBarLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiProgressBar>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetEmptyColor_FromGameThread(const glm::vec3& color);

    void SetFilledColor_FromGameThread(const glm::vec3& color);

    void SetOpacity_FromGameThread(const float opacity);

    void SetFillPercentValue_FromGrameThread(const float fillPercentValue);
};
} // namespace Scripts
} // namespace EngineCore
