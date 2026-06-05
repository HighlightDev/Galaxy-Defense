#include "CommonUiWidgetFactoryCreator.h"

#include "Core/CommonCore/Assertion.h"
#include "UiBackgroundOverlayReplicatorFactory.h"
#include "UiCanvasReplicatorFactory.h"
#include "UiDividerReplicatorFactory.h"
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
constexpr int32_t AsInt32(eCommonUiWidgetType type)
{
    return static_cast<typename std::underlying_type_t<eCommonUiWidgetType>>(type);
}
} // namespace

CommonUiWidgetFactoryCreator::CommonUiWidgetFactoryCreator()
{
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_OVERLAY)] = [] { return std::make_unique<UiOverlayReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_CANVAS)] = [] { return std::make_unique<UiCanvasReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_ITEM)] = [] { return std::make_unique<UiItemReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_RECTANGLE)]
        = [] { return std::make_unique<UiRectangleReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_IMAGE)] = [] { return std::make_unique<UiImageReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_LABEL)] = [] { return std::make_unique<UiLabelReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_TOGGLE_BUTTON)]
        = [] { return std::make_unique<UiToggleButtonReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_BACKGROUND_OVERLAY)]
        = [] { return std::make_unique<UiBackgroundOverlayReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_PROGRESS_BAR)]
        = [] { return std::make_unique<UiProgressBarReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_ROW_LAYOUT)]
        = [] { return std::make_unique<UiRowLayoutReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_SLIDER_BAR)] = [] { return std::make_unique<UiSliderReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_TEXT_BLOCK)]
        = [] { return std::make_unique<UiTextBlockReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_GRID_LAYOUT)]
        = [] { return std::make_unique<UiGridLayoutReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_SCROLL_LIST)]
        = [] { return std::make_unique<UiScrollListReplicatorFactory>(); };
    mFactoryProducers[AsInt32(eCommonUiWidgetType::UI_DIVIDER)] = [] { return std::make_unique<UiDividerReplicatorFactory>(); };
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
