#pragma once
#include "Component.h"
#include "Core/GameCore/Input/KeyboardBindings.h"
#include "Core/GameCore/Input/MouseBindings.h"

namespace EngineCore
{
   struct ComponentData;

   class InputComponent :
      public Component
   {

      KeyboardBindings m_keyboardBindings;

      MouseBindings m_mouseBindings;

   public:

      InputComponent(const ComponentData& componentData);

      virtual ~InputComponent();

      eComponentType GetComponentType() const override;

      // Game thread tick
      void Tick(const float deltaTime) override;

      void CollectDataForSerialization(SerializeDataContainer& dataContainer) override;

      std::vector<eKeyActionType> GetReleasedKeyActions();

      std::vector<eKeyActionType> GetPressedKeyActions();

      KeyboardBindings& GetKeyboardBindings();

      MouseBindings& GetMouseBindings();

      void SetIsReceivingMouseEvents(const bool receiveMouseEvents);

      void SetIsReceivingKeyboardEvents(const bool receiveKeyboardEvents);
   };

}
