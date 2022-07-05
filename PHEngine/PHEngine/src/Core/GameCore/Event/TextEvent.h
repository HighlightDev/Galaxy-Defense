#pragma once

#include <memory>

#include "TEvent.h"
#include "eTextEventEnums.h"
#include "Policy/Policies.h"
#include "Core/GameCore/GUI/Text/TextField.h"

using namespace EngineCore;

namespace Event
{
   class TextRegisterEvent
       : public TEvent<MultipleDataEventPolicy<std::shared_ptr<TextField>, eRegisterType>>
   {
   public:
      using Event_t = TEvent<MultipleDataEventPolicy<std::shared_ptr<TextField>, eRegisterType>>::Event_t;

      virtual std::string ToString() const override {
         return "TextRegisterEvent";
      }
   };

   class TextDataChangedEvent
       : public TEvent<MultipleDataEventPolicy<std::shared_ptr<TextField>, eTextChangedDataType>>
   {
   public:
      using Event_t = TEvent<MultipleDataEventPolicy<std::shared_ptr<TextField>, eTextChangedDataType>>::Event_t;

      virtual std::string ToString() const override {
         return "TextDataChangedEvent";
      }
   };
}