#pragma once

#include <cstdint>
#include <string>

namespace Game {

enum class eGameUiWidgetType : int32_t { UI_UPGRADE_ICON = 100 };

std::string GameUiWidgetTypeToString(const eGameUiWidgetType type);

} // namespace Game
