#include "PhysicsCollisionEvent.h"

namespace Event
{
   template class TEvent<PhysicsCollisionGameThreadEvent, eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<ePhysicsCollisionStateType, ePhysicsBodyType, int32_t /*this_phys_descriptor_id*/, int32_t /*this_actor_id*/, int32_t /*that_phys_descriptor_id*/, int32_t /*that_actor_id*/>>;
}