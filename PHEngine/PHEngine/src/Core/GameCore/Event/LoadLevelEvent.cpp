#include "LoadLevelEvent.h"

namespace Event
{
   template class TEvent<LoadLevelGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::string>>;
}