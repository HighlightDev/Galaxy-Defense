#include "CommonUiWidgetFactoryCreator.h"
#include "UiCanvasReplicatorFactory.h"
#include "UiOverlayReplicatorFactory.h"
#include "UiBackgroundOverlayReplicatorFactory.h"
#include "UiItemReplicatorFactory.h"
#include "UiRectangleReplicatorFactory.h"
#include "UiImageReplicatorFactory.h"
#include "UiLabelReplicatorFactory.h"
#include "UiToggleButtonReplicatorFactory.h"
#include "UiProgressBarReplicatorFactory.h"
#include "UiRowLayoutReplicatorFactory.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{
    namespace Scripts
    {
        std::unique_ptr<IReplicatorFactory> CommonUiWidgetFactoryCreator::GetReplicatorFactory(const eCommonUiWidgetType widgetType) const
        {
            switch (widgetType)
            {
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

            default:
                assert(false);
                return nullptr;
            }
        }
    }
}