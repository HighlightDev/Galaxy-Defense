#pragma once

#include "TEvent.h"
#include "Policy/Policies.h"
#include "Core/GameCore/GUI/Text/TextField.h"

using namespace EngineCore;

namespace Event
{
   enum class eRegisterType
   {
      REGISTER,
      UNREGISTER
   };

   class TextRegisterEvent
       : public TEvent<SingleDataEventPolicy<TextField *, eRegisterType>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<TextField *, eRegisterType>>::Event_t;
   };
}