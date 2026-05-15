#pragma once

#include <string>

namespace EngineCore {
namespace Scripts {
enum class eCommonUiWidgetType {
    UI_OVERLAY = 0,
    UI_CANVAS = 1,
    UI_ITEM = 2,
    UI_RECTANGLE = 3,
    UI_IMAGE = 4,
    UI_LABEL = 5,
    UI_TOGGLE_BUTTON = 6,
    UI_BACKGROUND_OVERLAY = 7,
    UI_PROGRESS_BAR = 8,
    UI_ROW_LAYOUT = 9,
    UI_SLIDER_BAR = 10,
    UI_TEXT_BLOCK = 11,
    UI_GRID_LAYOUT = 12,
    UI_SCROLL_LIST = 13
};

std::string CommonUiWidgetTypeToString(const eCommonUiWidgetType type);
} // namespace Scripts
} // namespace EngineCore
