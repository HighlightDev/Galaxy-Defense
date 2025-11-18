#include "CommonUiWidgetFactoryCreator.h"

#include "Core/CommonCore/Assertion.h"
#include "UiBackgroundOverlayReplicatorFactory.h"
#include "UiCanvasReplicatorFactory.h"
#include "UiGridLayoutReplicatorFactory.h"
#include "UiImageReplicatorFactory.h"
#include "UiItemReplicatorFactory.h"
#include "UiLabelReplicatorFactory.h"
#include "UiOverlayReplicatorFactory.h"
#include "UiProgressBarReplicatorFactory.h"
#include "UiRectangleReplicatorFactory.h"
#include "UiRowLayoutReplicatorFactory.h"
#include "UiSliderReplicatorFactory.h"
#include "UiTextBlockReplicatorFactory.h"
#include "UiToggleButtonReplicatorFactory.h"

namespace EngineCore {
namespace Scripts {
std::unique_ptr<IReplicatorFactory> CommonUiWidgetFactoryCreator::GetReplicatorFactory(const eCommonUiWidgetType widgetType) const
{
    switch (widgetType) {
    case eCommonUiWidgetType::UI_OVERLAY:
        return std::make_unique<UiOverlayReplicatorFactory>();
    case eCommonUiWidgetType::UI_CANVAS:
        return std::make_unique<UiCanvasReplicatorFactory>();
    case eCommonUiWidgetType::UI_ITEM:
        return std::make_unique<UiItemReplicatorFactory>();
    case eCommonUiWidgetType::UI_RECTANGLE:
        return std::make_unique<UiRectangleReplicatorFactory>();
    case eCommonUiWidgetType::UI_IMAGE:
        return std::make_unique<UiImageReplicatorFactory>();
    case eCommonUiWidgetType::UI_LABEL:
        return std::make_unique<UiLabelReplicatorFactory>();
    case eCommonUiWidgetType::UI_TOGGLE_BUTTON:
        return std::make_unique<UiToggleButtonReplicatorFactory>();
    case eCommonUiWidgetType::UI_BACKGROUND_OVERLAY:
        return std::make_unique<UiBackgroundOverlayReplicatorFactory>();
    case eCommonUiWidgetType::UI_PROGRESS_BAR:
        return std::make_unique<UiProgressBarReplicatorFactory>();
    case eCommonUiWidgetType::UI_ROW_LAYOUT:
        return std::make_unique<UiRowLayoutReplicatorFactory>();
    case eCommonUiWidgetType::UI_SLIDER_BAR:
        return std::make_unique<UiSliderReplicatorFactory>();
    case eCommonUiWidgetType::UI_TEXT_BLOCK:
        return std::make_unique<UiTextBlockReplicatorFactory>();
    case eCommonUiWidgetType::UI_GRID_LAYOUT:
        return std::make_unique<UiGridLayoutReplicatorFactory>();

    default:
        assert(false);
        return nullptr;
    }
}
} // namespace Scripts
} // namespace EngineCore