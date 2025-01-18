#include "RestartLevelEvent.h"

namespace Event
{
   template class TEvent<RestartLevelGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy>;
}