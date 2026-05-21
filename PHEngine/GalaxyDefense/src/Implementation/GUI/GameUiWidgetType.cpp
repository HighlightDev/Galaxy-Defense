#include "GameUiWidgetType.h"

namespace Game {

std::string GameUiWidgetTypeToString(const eGameUiWidgetType type)
{
    switch (type) {
    case eGameUiWidgetType::UI_UPGRADE_ICON:
        return "UI_UPGRADE_ICON";
    case eGameUiWidgetType::UI_CONNECTION_LINE:
        return "UI_CONNECTION_LINE";
    default:
        return "UNKNOWN_GAME_UI_TYPE";
    }
}

} // namespace Game
