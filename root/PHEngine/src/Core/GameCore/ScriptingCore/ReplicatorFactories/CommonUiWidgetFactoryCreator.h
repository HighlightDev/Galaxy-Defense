#pragma once

#include "CommonUiWidgetType.h"
#include "IReplicatorFactory.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>

namespace EngineCore {
namespace Scripts {
class CommonUiWidgetFactoryCreator {
    using FactoryProducer = std::function<std::unique_ptr<IReplicatorFactory>()>;

    std::unordered_map<int32_t, FactoryProducer> mFactoryProducers;

    CommonUiWidgetFactoryCreator();

public:
    static CommonUiWidgetFactoryCreator& GetInstance();

    void RegisterFactory(const int32_t widgetType, FactoryProducer factoryProducer);

    std::unique_ptr<IReplicatorFactory> GetReplicatorFactory(const int32_t widgetType) const;
};
} // namespace Scripts
} // namespace EngineCore
