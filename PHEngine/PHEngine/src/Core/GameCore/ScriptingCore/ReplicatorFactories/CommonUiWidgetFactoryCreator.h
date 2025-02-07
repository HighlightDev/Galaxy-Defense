#pragma once

#include "CommonUiWidgetType.h"
#include "IReplicatorFactory.h"

#include <memory>

namespace EngineCore {
namespace Scripts {
class CommonUiWidgetFactoryCreator {
public:
    std::unique_ptr<IReplicatorFactory> GetReplicatorFactory(const eCommonUiWidgetType widgetType) const;
};
} // namespace Scripts
} // namespace EngineCore