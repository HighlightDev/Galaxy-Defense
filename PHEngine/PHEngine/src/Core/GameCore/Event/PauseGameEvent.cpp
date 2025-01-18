#include "PauseGameEvent.h"

namespace Event
{
   template class TEvent<PauseGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<bool>>;
}
