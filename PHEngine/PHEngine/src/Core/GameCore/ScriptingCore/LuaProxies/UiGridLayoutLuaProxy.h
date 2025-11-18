#pragma once

#include "UiItemLuaProxy.h"

namespace EngineCore {
namespace GUI {
class UiGridLayout;
}
} // namespace EngineCore

using namespace EngineCore::GUI;

namespace EngineCore {
namespace Scripts {
class UiGridLayoutLuaProxy : public UiItemLuaProxy {
protected:
    uint32_t mHorizontalSpacing{0};

    uint32_t mVerticalSpacing{0};

    uint32_t mColumnsCount{1};

    uint32_t mRowsCount{1};

public:
    explicit UiGridLayoutLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiGridLayout>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetHorizontalSpacing_FromGameThread(const uint32_t value);

    void SetVerticalSpacing_FromGameThread(const uint32_t value);

    void SetColumnsCount_FromGameThread(const uint32_t count);

    void SetRowsCount_FromGameThread(const uint32_t count);
};
} // namespace Scripts
} // namespace EngineCore
