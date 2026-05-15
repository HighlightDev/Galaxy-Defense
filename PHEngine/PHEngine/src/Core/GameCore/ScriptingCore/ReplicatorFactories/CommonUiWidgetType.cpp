#include "CommonUiWidgetType.h"

namespace EngineCore::Scripts {

std::string CommonUiWidgetTypeToString(const eCommonUiWidgetType type)
{
    switch (type) {
    case eCommonUiWidgetType::UI_OVERLAY:
        return "UI_OVERLAY";
    case eCommonUiWidgetType::UI_CANVAS:
        return "UI_CANVAS";
    case eCommonUiWidgetType::UI_ITEM:
        return "UI_ITEM";
    case eCommonUiWidgetType::UI_RECTANGLE:
        return "UI_RECTANGLE";
    case eCommonUiWidgetType::UI_IMAGE:
        return "UI_IMAGE";
    case eCommonUiWidgetType::UI_LABEL:
        return "UI_LABEL";
    case eCommonUiWidgetType::UI_TOGGLE_BUTTON:
        return "UI_TOGGLE_BUTTON";
    case eCommonUiWidgetType::UI_BACKGROUND_OVERLAY:
        return "UI_BACKGROUND_OVERLAY";
    case eCommonUiWidgetType::UI_PROGRESS_BAR:
        return "UI_PROGRESS_BAR";
    case eCommonUiWidgetType::UI_ROW_LAYOUT:
        return "UI_ROW_LAYOUT";
    case eCommonUiWidgetType::UI_SLIDER_BAR:
        return "UI_SLIDER_BAR";
    case eCommonUiWidgetType::UI_TEXT_BLOCK:
        return "UI_TEXT_BLOCK";
    case eCommonUiWidgetType::UI_GRID_LAYOUT:
        return "UI_GRID_LAYOUT";
    case eCommonUiWidgetType::UI_SCROLL_LIST:
        return "UI_SCROLL_LIST";
    default:
        return "UNKNOWN_TYPE";
    }
}
} // namespace EngineCore::Scripts
