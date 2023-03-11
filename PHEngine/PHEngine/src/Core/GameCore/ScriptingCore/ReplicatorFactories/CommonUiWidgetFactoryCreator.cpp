#include "CommonUiWidgetFactoryCreator.h"
#include "UiCanvasReplicatorFactory.h"
#include "Core/CommonCore/Assertion.h"

namespace EngineCore
{
    namespace Scripts
    {
        std::unique_ptr<IReplicatorFactory> CommonUiWidgetFactoryCreator::GetReplicatorFactory(const eCommonUiWidgetType widgetType) const
        {
            switch (widgetType)
            {
            case eCommonUiWidgetType::UI_CANVAS:
                return std::make_unique<UiCanvasReplicatorFactory>();

            default:
                assert(false);
                return nullptr;
            }
        }
    }
}