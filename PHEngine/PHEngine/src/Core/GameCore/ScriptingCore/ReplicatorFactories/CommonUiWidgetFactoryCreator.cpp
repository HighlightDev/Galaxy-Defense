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
#include "UiScrollListReplicatorFactory.h"
#include "UiSliderReplicatorFactory.h"
#include "UiTextBlockReplicatorFactory.h"
#include "UiToggleButtonReplicatorFactory.h"

namespace EngineCore {
namespace Scripts {

namespace {
constexpr int32_t AsInt(eCommonUiWidgetType type)
{
    return static_cast<int32_t>(type);
}
} // namespace

CommonUiWidgetFactoryCreator::CommonUiWidgetFactoryCreator()
{
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_OVERLAY)] = [] { return std::make_unique<UiOverlayReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_CANVAS)] = [] { return std::make_unique<UiCanvasReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_ITEM)] = [] { return std::make_unique<UiItemReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_RECTANGLE)] = [] { return std::make_unique<UiRectangleReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_IMAGE)] = [] { return std::make_unique<UiImageReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_LABEL)] = [] { return std::make_unique<UiLabelReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_TOGGLE_BUTTON)]
        = [] { return std::make_unique<UiToggleButtonReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_BACKGROUND_OVERLAY)]
        = [] { return std::make_unique<UiBackgroundOverlayReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_PROGRESS_BAR)]
        = [] { return std::make_unique<UiProgressBarReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_ROW_LAYOUT)]
        = [] { return std::make_unique<UiRowLayoutReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_SLIDER_BAR)] = [] { return std::make_unique<UiSliderReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_TEXT_BLOCK)]
        = [] { return std::make_unique<UiTextBlockReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_GRID_LAYOUT)]
        = [] { return std::make_unique<UiGridLayoutReplicatorFactory>(); };
    mFactoryProducers[AsInt(eCommonUiWidgetType::UI_SCROLL_LIST)]
        = [] { return std::make_unique<UiScrollListReplicatorFactory>(); };
}

CommonUiWidgetFactoryCreator& CommonUiWidgetFactoryCreator::GetInstance()
{
    static CommonUiWidgetFactoryCreator instance;
    return instance;
}

void CommonUiWidgetFactoryCreator::RegisterFactory(const int32_t widgetType, FactoryProducer factoryProducer)
{
    mFactoryProducers[widgetType] = std::move(factoryProducer);
}

std::unique_ptr<IReplicatorFactory> CommonUiWidgetFactoryCreator::GetReplicatorFactory(const int32_t widgetType) const
{
    const auto it = mFactoryProducers.find(widgetType);
    ext_assert(
        it != mFactoryProducers.end(),
        "CommonUiWidgetFactoryCreator::GetReplicatorFactory: No factory registered for widget type "
            + std::to_string(widgetType));
    return it->second();
}

} // namespace Scripts
} // namespace EngineCore
