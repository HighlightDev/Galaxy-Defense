#include "CommonUiWidgetFactoryCreator.h"
#include "UiCanvasReplicatorFactory.h"
#include "UiOverlayReplicatorFactory.h"
#include "UiItemReplicatorFactory.h"
#include "UiRectangleReplicatorFactory.h"
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

            default:
                assert(false);
                return nullptr;
            }
        }
    }
}