#pragma once

#include <memory>

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
       : public TEvent<SingleDataEventPolicy<std::shared_ptr<TextField>, eRegisterType>>
   {
   public:
      using Event_t = TEvent<SingleDataEventPolicy<std::shared_ptr<TextField>, eRegisterType>>::Event_t;
   };
}