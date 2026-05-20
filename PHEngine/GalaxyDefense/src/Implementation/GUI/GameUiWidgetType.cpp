#include "GameUiWidgetType.h"

namespace Game {

std::string GameUiWidgetTypeToString(const eGameUiWidgetType type)
{
    switch (type) {
    case eGameUiWidgetType::UI_UPGRADE_ICON:
        return "UI_UPGRADE_ICON";
    default:
        return "UNKNOWN_GAME_UI_TYPE";
    }
}

} // namespace Game
