#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsBodyType.h"

#include <string>

using namespace EnginePhysics;

namespace Event
{
   enum class ePhysicsCollisionStateType
   {
      COLLISION_REGISTERED,
      COLLISION_UNREGISTER
   };

   using ParentEvent_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<ePhysicsCollisionStateType,
                                                        ePhysicsBodyType,
                                                        int32_t /*this_phys_descriptor_id*/,
                                                        int32_t /*this_actor_id*/,
                                                        int32_t /*that_phys_descriptor_id*/,
                                                        int32_t /*that_actor_id*/>>;

   class PhysicsCollisionGameThreadEvent
       : public ParentEvent_t
   {
   public:
      using Event = ParentEvent_t::Event_t;

      std::string ToString() const override
      {
         return "PhysicsCollisionGameThreadEvent";
      }
   };
}