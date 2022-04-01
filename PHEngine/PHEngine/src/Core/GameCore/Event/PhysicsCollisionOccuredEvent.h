#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsBodyType.h"

#include <string>

using namespace EnginePhysics;

namespace Event
{
   using ParentEvent_t = TEvent<SingleDataEventPolicy<ePhysicsBodyType, uint32_t, uint32_t>>;

   class PhysicsCollisionOccuredEvent
      : public ParentEvent_t
   {
   public:
      using Event = ParentEvent_t::Event_t;
         
   };
}