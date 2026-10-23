#pragma once

#include "Core/GameCore/ScriptingCore/LuaProxies/UiProxies/UiItemBaseLuaProxy.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace EngineCore::GUI {
class UiConnectionLine;
} // namespace EngineCore::GUI

namespace Game {

class UiConnectionLineLuaProxy : public ::EngineCore::Scripts::UiItemBaseLuaProxy {
protected:
    glm::vec2 mStartPoint;
    glm::vec2 mEndPoint;

    std::string mStartAnchorTarget;
    std::string mEndAnchorTarget;

    glm::vec3 mColor;

    float mThicknessPx;
    float mDashLengthPx;
    float mGapLengthPx;
    float mOpacity;

public:
    explicit UiConnectionLineLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiConnectionLine>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetStartPoint_FromGameThread(const glm::vec2& startPoint);

    void SetEndPoint_FromGameThread(const glm::vec2& endPoint);

    void SetStartAnchorTarget_FromGameThread(const std::string& targetUiItemName);

    void SetEndAnchorTarget_FromGameThread(const std::string& targetUiItemName);

    void SetColor_FromGameThread(const glm::vec3& color);

    void SetThicknessPx_FromGameThread(const float thicknessPx);

    void SetDashLengthPx_FromGameThread(const float dashLengthPx);

    void SetGapLengthPx_FromGameThread(const float gapLengthPx);

    void SetOpacity_FromGameThread(const float opacity);
};

} // namespace Game
