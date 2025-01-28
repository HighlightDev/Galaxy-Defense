#include "ChangeGameModeEvent.h"

namespace Event
{
    template class TEvent<ChangeGameModeEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eGameModeType>>;

    template class TEvent<LuaChangeGameModeEvent, eEventThreadType::LUA_THREAD, SingleDataEventPolicy<eGameModeType>>;
}
