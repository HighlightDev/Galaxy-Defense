#include "ChangeEditModeEvent.h"

namespace Event
{
    template class TEvent<ChangeEditModeEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eEditModeType>>;
}
