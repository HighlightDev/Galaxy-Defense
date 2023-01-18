#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"
#include "Core/GameCore/Actor.h"
#include "Implementation/Actors/MissileActor.h"

#include <memory>

using namespace Game;
using namespace EngineCore;

namespace Event
{
   class RayCollisionEvent
       : public TEvent<MultipleDataEventPolicy<std::weak_ptr<MissileActor>/*event sender actor*/, std::weak_ptr<Actor>/*collided actor*/>>
   {
   public:
      using Event_t = TEvent<MultipleDataEventPolicy<std::weak_ptr<MissileActor>, std::weak_ptr<Actor>>>::Event_t;

      std::string ToString() const override
      {
         return "RayCollisionEvent";
      }
   };

}