#include "ChangeGameModeEvent.h"

namespace Event
{
    template class TEvent<ChangeGameModeEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eGameModeType>>;
}
