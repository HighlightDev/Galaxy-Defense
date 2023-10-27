#include "ChangeGameModeEvent.h"

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eGameModeType>>;
}
