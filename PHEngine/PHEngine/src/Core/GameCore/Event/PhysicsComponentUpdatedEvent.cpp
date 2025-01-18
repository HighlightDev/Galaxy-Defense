#include "PhysicsComponentUpdatedEvent.h"

namespace Event
{
   template class TEvent<PhysicsComponentUpdatedGameThreadEvent, eEventThreadType::GAME_THREAD, SingleDataEventPolicy<std::string>>;
}