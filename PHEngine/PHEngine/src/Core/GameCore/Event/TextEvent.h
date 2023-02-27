#pragma once

#include <memory>

#include "TEvent.h"
#include "eTextEventEnums.h"
#include "Policy/Policies.h"
#include "Core/GameCore/GUI/HudText/HudTextField.h"

using namespace EngineCore;

namespace Event
{
   class TextRegisterEvent
       : public TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::shared_ptr<HudTextField>, eRegisterType, bool>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::shared_ptr<HudTextField>, eRegisterType, bool>>::Event_t;

      std::string ToString() const override {
         return "GameThreadTextRegisterEvent";
      }
   };

   class TextDataChangedEvent
       : public TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::weak_ptr<HudTextField>, eTextChangedDataType>>
   {
   public:
      using Event_t = TEvent<eEventThreadType::GAME_THREAD, MultipleDataEventPolicy<std::weak_ptr<HudTextField>, eTextChangedDataType>>::Event_t;

      std::string ToString() const override {
         return "GameThreadTextDataChangedEvent";
      }
   };
}