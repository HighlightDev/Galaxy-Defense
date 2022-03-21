#pragma once

#include "Core/GameCore/Event/TEvent.h"
#include "Core/GameCore/Event/Policy/Policies.h"
#include "Implementation/MainPlayerActionEnum.h"

using namespace Game;

namespace Event
{
    class MainPlayerActionEvent
      : public TEvent<SingleDataEventPolicy<eMainPlayerActionEnum>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<eMainPlayerActionEnum>>::Event_t;
   };

}