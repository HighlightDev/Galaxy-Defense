#pragma once

#include "UiItemBaseLuaProxy.h"

#include <glm/vec3.hpp>

namespace EngineCore {
namespace GUI {
class UiScrollBar;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiScrollBarLuaProxy : public UiItemBaseLuaProxy {
protected:
    float mMaxScrollValue{0.0f};
    float mMinScrollValue{0.0f};
    float mScrollValue{0.0f};
    float mScrollStep{0.0f};

public:
    explicit UiScrollBarLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiScrollBar>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetMaxScrollValue_FromGameThread(const float maxScrollValue);

    void SetMinScrollValue_FromGameThread(const float minScrollValue);

    void SetScrollValue_FromGameThread(const float scrollValue);

    void SetScrollStep_FromGameThread(const float scrollStep);
};
} // namespace Scripts
} // namespace EngineCore
