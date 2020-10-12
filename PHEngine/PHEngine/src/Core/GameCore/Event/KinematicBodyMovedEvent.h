#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/Components/Transform.h"
#include "Core/GameCore/Physics/PhysicsDescriptors/PhysicsDescriptor.h"

namespace Event
{

   class KinematicBodyMovedEvent
      : public TEvent<AtomicEventPolicy<EnginePhysics::PhysicsDescriptor*, Game::Transform>>
   {
   public:
      using Event_t = TEvent<AtomicEventPolicy<EnginePhysics::PhysicsDescriptor*, Game::Transform>>::Event_t;
   };

}