#pragma once

#include "UiItemLuaProxy.h"

namespace EngineCore {
namespace GUI {
class UiScrollList;
}
} // namespace EngineCore

namespace EngineCore {
namespace Scripts {
class UiScrollListLuaProxy : public UiItemLuaProxy {
    uint32_t mSpacing{0};
    int32_t mScrollOffset{0};
    int32_t mScrollSpeed{30};

public:
    explicit UiScrollListLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiScrollList>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetSpacing_FromGameThread(const uint32_t value);

    void SetScrollOffset_FromGameThread(const int32_t value);

    void SetScrollSpeed_FromGameThread(const int32_t value);
};
} // namespace Scripts
} // namespace EngineCore
