#pragma once

#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiRectangle;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiRectangleLuaProxy : public UiItemBaseLuaProxy {
protected:
    glm::vec3 mColor;

    float mOpacity;

    float mBorderRadius;

    bool mIsRoundTop;

    bool mIsRoundBottom;

public:
    explicit UiRectangleLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiRectangle>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetColor_FromGameThread(const glm::vec3& color);

    void SetOpacity_FromGameThread(const float opacity);

    void SetBorderRadius_FromGameThread(const float borderRadius);

    void SetIsRoundTop_FromGameThread(const bool bIsRoundTop);

    void SetIsRoundBottom_FromGameThread(const bool bIsRoundBottom);
};
} // namespace Scripts
} // namespace EngineCore
