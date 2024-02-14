#include "ChangeEditModeEvent.h"

namespace Event
{
    template class TEvent<eEventThreadType::GAME_THREAD, SingleDataEventPolicy<eEditModeType>>;
}
