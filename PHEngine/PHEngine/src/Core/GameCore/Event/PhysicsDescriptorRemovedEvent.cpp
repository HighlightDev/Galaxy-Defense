#include "PhysicsDescriptorRemovedEvent.h"

namespace Event
{
   template class TEvent<PhysicsDescriptorRemovedGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<size_t>>;
}