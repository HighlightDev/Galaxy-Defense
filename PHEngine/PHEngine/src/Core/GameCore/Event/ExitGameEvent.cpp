#include "ExitGameEvent.h"

namespace Event
{
   template class TEvent<ExitGameThreadEvent, eEventThreadType::GAME_THREAD, NoDataEventPolicy>;
}
