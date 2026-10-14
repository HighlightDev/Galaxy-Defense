#pragma once

#include "UiItemLuaProxy.h"

#include <glm/vec3.hpp>

#include <cstdint>

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
    uint8_t mScrollbarSide{0};
    glm::vec3 mScrollbarBackgroundColor{0.2f, 0.2f, 0.2f};
    glm::vec3 mScrollbarThumbColor{0.6f, 0.6f, 0.6f};
    uint32_t mScrollbarThicknessPixels{8};

public:
    explicit UiScrollListLuaProxy(const std::shared_ptr<::EngineCore::GUI::UiScrollList>& ownerUiItem);

    void OnLuaThreadDataUpdated(const std::string& jsonParameters) override;

    std::string GetGameThreadData() override;

    void SetSpacing_FromGameThread(const uint32_t value);

    void SetScrollOffset_FromGameThread(const int32_t value);

    void SetScrollSpeed_FromGameThread(const int32_t value);

    void SetScrollbarSide_FromGameThread(const uint8_t value);

    void SetScrollbarBackgroundColor_FromGameThread(const glm::vec3& value);

    void SetScrollbarThumbColor_FromGameThread(const glm::vec3& value);

    void SetScrollbarThicknessPixels_FromGameThread(const uint32_t value);
};
} // namespace Scripts
} // namespace EngineCore
