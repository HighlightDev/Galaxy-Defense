#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsBodyType.h"

#include <string>

using namespace EnginePhysics;

namespace Event
{
   using ParentEvent_t = TEvent<SingleDataEventPolicy<ePhysicsBodyType,
                                                      uint32_t /*this_phys_descriptor_id*/,
                                                      uint64_t /*this_actor_id*/,
                                                      uint32_t /*that_phys_descriptor_id*/,
                                                      uint64_t /*that_actor_id*/>>;

   class PhysicsCollisionOccuredEvent
       : public ParentEvent_t
   {
   public:
      using Event = ParentEvent_t::Event_t;

      virtual std::string ToString() const override {
         return "PhysicsCollisionOccuredEvent";
      }
   };
}