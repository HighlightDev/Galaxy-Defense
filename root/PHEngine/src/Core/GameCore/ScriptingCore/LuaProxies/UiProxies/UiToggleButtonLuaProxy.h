#pragma once

#include "Core/GameCore/GUI/Common/TextEnums.h"
#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

#include <string>

namespace EngineCore {
namespace GUI {
class UiToggleButton;
}
} // namespace EngineCore

using namespace EngineCore;

namespace EngineCore {
namespace Scripts {
class UiToggleButtonLuaProxy : public UiItemBaseLuaProxy {
protected:
    glm::vec3 mToggleOffColor;

    glm::vec3 mToggleOnColor;

    float mOpacity;

    bool mIsStateOn;

public:
    explicit UiToggleButtonLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiToggleButton>& ownerUiItem);

    void EnableMouseInputReceiver();

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetToggleOnColor_FromGameThread(const glm::vec3& color);

    void SetToggleOffColor_FromGameThread(const glm::vec3& color);

    void SetOpacity_FromGameThread(const float opacity);

    void SetIsStateOn_FromGameThread(const bool isStateOn);
};
} // namespace Scripts
} // namespace EngineCore
